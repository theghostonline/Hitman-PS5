// Routes. Payload shapes follow the network map (README.md sections 3-4) of hitman-ps5-net.
#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "hs.h"

// Paths are served in the same form as the client's own hard-coded fallbacks (network map
// section 2 code map, metrics default at 0xc25324), so a client that falls back to its
// built-in paths against a redirected host still lands on these routes.
#define PATH_OAUTH         "/oauth/token"
#define PATH_USERCHANNEL   "authentication/api/userchannel"
#define PATH_CONFIGURATION "authentication/api/configuration"
#define PATH_METRICS       "api/metrics/add"
#define PATH_RESOURCES     "resources"

static json_t *g_seed;  // extensions every new profile starts with (seed_extensions file)

int app_init(void) {
  if (!g_cfg.seed_extensions[0]) return 0;
  json_error_t err;
  g_seed = json_load_file(g_cfg.seed_extensions, 0, &err);
  if (!g_seed || !json_is_object(g_seed)) {
    LOGE("seed_extensions %s: must be a JSON object of { extensionName: value } (%s)", g_cfg.seed_extensions,
         g_seed ? "not an object" : err.text);
    json_decref(g_seed);
    g_seed = NULL;
    return -1;
  }
  LOGI("new profiles start with %zu seed extensions from %s", json_object_size(g_seed), g_cfg.seed_extensions);
  return 0;
}

// ------------------------------------------------------------------ helpers

static bool valid_host(const char *h) {
  size_t n = strlen(h);
  if (n == 0 || n > 200) return false;
  for (const char *p = h; *p; p++) {
    if (!isalnum((unsigned char)*p) && !strchr(".-:[]", *p)) return false;
  }
  return true;
}

// The base URL advertised to the game: public_url if set, else the Host the game used to reach
// this request (127.0.0.1:8000 after the in-memory patch, or a LAN address when testing).
static void base_url(const http_req_t *req, char *out, size_t n) {
  if (g_cfg.public_url[0]) {
    snprintf(out, n, "%s", g_cfg.public_url);
    return;
  }
  const char *host = http_header(req, "Host");
  if (host && valid_host(host)) snprintf(out, n, "http://%s", host);
  else snprintf(out, n, "http://127.0.0.1:%d", g_cfg.port);
}

static bool bearer_token(const http_req_t *req, char *out, size_t n) {
  const char *a = http_header(req, "Authorization");
  if (!a || !str_iprefix(a, "bearer ")) return false;
  a += 7;
  while (*a == ' ') a++;
  if (!*a || strlen(a) >= n) return false;
  snprintf(out, n, "%s", a);
  return true;
}

// Resolves the caller's profile from its bearer token. The game only sends the token over https
// (sub_c43dee checks the URL scheme, docs/PS5_GHIDRA_FINDINGS.md) and the patch makes it http, so
// requests from the console itself use the token the game got from its own /oauth/token instead.
// After a server restart that token is not in memory yet; the console's most recent login is
// used then. LAN clients must send a valid token: nothing on the network can act as the console.
static bool require_user(http_req_t *req, http_res_t *res, char user_id[HS_ID_LEN + 1]) {
  char token[4096];
  bool have = bearer_token(req, token, sizeof(token)) || (req->peer_loopback && proxy_session_token(token, sizeof(token)));
  int rc = have ? db_user_for_token(token, user_id) : 1;
  if (rc < 0) {
    http_res_error(res, 500, "server_error", NULL);
    return false;
  }
  if (rc == 0) return true;
  if (req->peer_loopback) {
    rc = db_user_for_latest_token(user_id);
    if (rc == 0) {
      LOGI("%s: no live session token, using the console's most recent login (profile %s)", req->path, user_id);
      return true;
    }
    if (rc < 0) {
      http_res_error(res, 500, "server_error", NULL);
      return false;
    }
  }
  LOGW("%s from %s: no valid bearer token and no login on this console yet", req->path, req->peer);
  http_res_error(res, 401, "unauthorized", "log in with /oauth/token first");
  return false;
}

static json_t *json_body(http_req_t *req, http_res_t *res) {
  if (req->body_len == 0) return json_object();
  json_error_t err;
  json_t *v = json_loadb(req->body, req->body_len, 0, &err);
  if (!v) {
    http_res_error(res, 400, "invalid_json", err.text);
    return NULL;
  }
  if (!json_is_object(v)) {
    json_decref(v);
    http_res_error(res, 400, "invalid_json", "body must be a JSON object");
    return NULL;
  }
  return v;
}

// String member, or NULL when absent, not a string, or empty.
static const char *jstr(json_t *obj, const char *key) {
  json_t *v = json_object_get(obj, key);
  const char *s = json_is_string(v) ? json_string_value(v) : NULL;
  return s && *s ? s : NULL;
}

static void body_preview(const http_req_t *req, char *out, size_t n) {
  size_t o = 0;
  for (size_t i = 0; i < req->body_len && o + 4 < n; i++) {
    unsigned char ch = (unsigned char)req->body[i];
    out[o++] = (ch >= 32 && ch < 127) ? (char)ch : '.';
  }
  if (req->body_len > 0 && o + 4 >= n) {
    out[o++] = '.';
    out[o++] = '.';
    out[o++] = '.';
  }
  out[o] = '\0';
}

static void not_found(http_req_t *req, http_res_t *res) {
  char preview[400];
  body_preview(req, preview, sizeof(preview));
  const char *ct = http_header(req, "Content-Type");
  const char *host = http_header(req, "Host");
  const char *ua = http_header(req, "User-Agent");
  const char *accept = http_header(req, "Accept");
  LOGW("no handler for %s %s%s%s (host %s, accept %s, content-type %s, ua %s, %zu byte body) %s", req->method,
       req->path, req->query[0] ? "?" : "", req->query, host ? host : "-", accept ? accept : "-", ct ? ct : "-",
       ua ? ua : "-", req->body_len, preview);
  http_res_error(res, 404, "not_found", req->path);
}

static bool method_is(http_req_t *req, http_res_t *res, const char *method) {
  if (!strcmp(req->method, method)) return true;
  http_res_error(res, 405, "method_not_allowed", method);
  return false;
}

// ------------------------------------------------------------------ 4.1 config

static bool valid_segment(const char *s) {
  size_t n = strlen(s);
  if (n == 0 || n > 64) return false;
  for (; *s; s++) {
    if (!isalnum((unsigned char)*s) && !strchr("_.-", *s)) return false;
  }
  return true;
}

static void h_config(http_req_t *req, http_res_t *res, const char *name, const char *version) {
  if (!method_is(req, res, "GET")) return;
  if (!valid_segment(name) || !valid_segment(version)) {
    http_res_error(res, 400, "invalid_request", "bad config name or version");
    return;
  }
  char issuer[128] = "";
  http_form_get(req->query, "issuer", issuer, sizeof(issuer));
  LOGI("config: name=%s client=%s issuer=%s -> GAME_VER %s", name, version, issuer[0] ? issuer : "-", g_cfg.game_ver);

  char base[300], online_cfg[400], agreement[400], resources[400], audience[96];
  base_url(req, base, sizeof(base));
  // Matches Peacock's /config/:audience/:serverVersion (components/index.ts): Configuration.Url is
  // the onlineconfig.json file, OAuthPath has no leading slash, and the Online.* tunables live in
  // that file, not here. ISSUER_ID echoes the ?issuer= query (or "*").
  snprintf(online_cfg, sizeof(online_cfg), "%s/files/onlineconfig.json", base);
  snprintf(agreement, sizeof(agreement), "%s/files/privacypolicy/hm3/privacypolicy.json", base);
  snprintf(resources, sizeof(resources), "%s/files", base);
  snprintf(audience, sizeof(audience), "%s_8", name);  // e.g. ps4-prod_8

  // Name must equal the client's online_VersionConfigName (strcmp in sub_c56280), so echo it.
  json_t *entry = json_pack(
      "{s:s, s:s, s:{s:{s:s, s:s}, s:{s:s, s:s, s:s}, s:{s:s, s:s}, s:{s:s, s:s, s:s}, s:{s:s}}, s:s}",
      "Name", name,
      "GAME_VER", g_cfg.game_ver,
      "SERVER_VER",
        "Metrics", "MetricsServerHost", base, "MetricsServerServicePath", PATH_METRICS,
        "Authentication", "AuthenticationHost", base, "UserChannelServicePath", PATH_USERCHANNEL,
                          "AuthenticationServicePathConfiguration", PATH_CONFIGURATION,
        "Configuration", "Url", online_cfg, "AgreementUrl", agreement,
        "GlobalAuthentication", "AuthenticationHost", base, "OAuthPath", "oauth/token",
                                "RequestedAudience", audience,
        "Resources", "ResourcesServicePath", resources,
      "ISSUER_ID", issuer[0] ? issuer : "*");
  http_res_json(res, 200, json_pack("{s:[o]}", "Versions", entry));
}

// ------------------------------------------------------------------ 4.2/4.3 OAuth

typedef struct {
  const char *form;  // x-www-form-urlencoded body, or NULL
  json_t     *json;  // JSON body, or NULL
} oauth_body_t;

static bool field(const oauth_body_t *b, const char *key, char *out, size_t n) {
  if (b->json) {
    json_t *v = json_object_get(b->json, key);
    if (json_is_string(v)) snprintf(out, n, "%s", json_string_value(v));
    else if (json_is_integer(v)) snprintf(out, n, "%lld", (long long)json_integer_value(v));
    else return false;
    return true;
  }
  return http_form_get(b->form, key, out, n);
}

// Optional field: NULL when absent or empty.
static const char *opt(const oauth_body_t *b, const char *key, char *buf, size_t n) {
  return field(b, key, buf, n) && buf[0] ? buf : NULL;
}

static void send_tokens(http_res_t *res, const char *access, const char *refresh) {
  http_res_json(res, 200,
                json_pack("{s:s, s:s, s:i, s:s}", "access_token", access, "token_type", "bearer",
                          "expires_in", g_cfg.token_ttl, "refresh_token", refresh));
}

static void log_oauth_fields(const oauth_body_t *b) {
  if (!b->form) return;
  // Field meanings are partly open (network map section 8), so record what the console sends,
  // with every value outside a short allowlist redacted (http_redact_form).
  char line[1024];
  http_redact_form(b->form, line, sizeof(line));
  LOGI("oauth fields: %s", line);
}

static void h_oauth(http_req_t *req, http_res_t *res) {
  if (!method_is(req, res, "POST")) return;
  oauth_body_t b = {0};
  // The HITMAN client sends Content-Type: application/json but the body is actually
  // form-urlencoded (Peacock: "the game is a LIAR and does NOT give us application/json"). So the
  // real format is detected from the body, not the header: JSON only when it starts with '{'.
  const char *body = req->body ? req->body : "";
  while (*body == ' ' || *body == '\r' || *body == '\n' || *body == '\t') body++;
  if (*body == '{') {
    json_error_t err;
    b.json = json_loadb(req->body, req->body_len, 0, &err);
    if (!json_is_object(b.json)) {
      json_decref(b.json);
      http_res_error(res, 400, "invalid_request", "body is not a JSON object");
      return;
    }
  } else {
    b.form = req->body;
  }
  log_oauth_fields(&b);

  char grant[64] = "";
  field(&b, "grant_type", grant, sizeof(grant));
  char user_id[HS_ID_LEN + 1], access[HS_TOKEN_LEN + 1], refresh[HS_TOKEN_LEN + 1];

  if (!strcmp(grant, "external_psn")) {
    char account[129], online_buf[129], issuer_buf[65], region_buf[65], locale_buf[65];
    const char *account_id = opt(&b, "psn_userid", account, sizeof(account));
    if (!account_id) {
      http_res_error(res, 400, "invalid_request", "psn_userid is required");
      goto done;
    }
    const char *online_id = opt(&b, "psn_onlineid", online_buf, sizeof(online_buf));
    const char *issuer_id = opt(&b, "psn_issuerid", issuer_buf, sizeof(issuer_buf));
    const char *region = opt(&b, "rgn", region_buf, sizeof(region_buf));
    const char *locale = opt(&b, "locale", locale_buf, sizeof(locale_buf));
    bool created = false;
    if (db_login_psn(account_id, online_id, issuer_id, region, locale, g_seed, user_id, &created) ||
        db_issue_tokens(user_id, access, refresh)) {
      http_res_error(res, 500, "server_error", NULL);
      goto done;
    }
    LOGI("login: psn_userid=%s psn_onlineid=%s -> profile %s%s", account_id, online_id ? online_id : "-", user_id,
         created ? " (new)" : "");
    if (req->peer_loopback) proxy_remember_token(access);
    send_tokens(res, access, refresh);
  } else if (!strcmp(grant, "refresh_token")) {
    char old[256];
    if (!field(&b, "refresh_token", old, sizeof(old)) || !old[0]) {
      http_res_error(res, 400, "invalid_request", "refresh_token is required");
      goto done;
    }
    int rc = db_refresh_tokens(old, user_id, access, refresh);
    if (rc > 0) {
      http_res_error(res, 400, "invalid_grant", "unknown or expired refresh token");
    } else if (rc < 0) {
      http_res_error(res, 500, "server_error", NULL);
    } else {
      LOGI("refresh: profile %s", user_id);
      if (req->peer_loopback) proxy_remember_token(access);
      send_tokens(res, access, refresh);
    }
  } else {
    LOGW("oauth: unsupported grant_type \"%s\"", grant);
    http_res_error(res, 400, "unsupported_grant_type", grant[0] ? grant : NULL);
  }
done:
  json_decref(b.json);
}

// ------------------------------------------------------------------ 4.4 game auth Init

static void h_init(http_req_t *req, http_res_t *res) {
  if (!method_is(req, res, "GET")) return;
  // Init comes right after /oauth/token and without a bearer header (http, see require_user).
  char user_id[HS_ID_LEN + 1];
  if (!require_user(req, res, user_id)) return;
  LOGI("init: profile %s (%s)", user_id, req->query);

  char base[300], bloburl[400], now[40], token[160];
  base_url(req, base, sizeof(base));
  // Matches Peacock's Init (components/index.ts generateBlobConfig): bloburl ends in the
  // resources-{version} path the client appends rpkg names to, blobsig is non-empty, and there is
  // a token field. An empty blobsig made HITMAN reject the config.
  snprintf(bloburl, sizeof(bloburl), "%s/resources-8-25/", base);
  snprintf(token, sizeof(token), "%ld-%ld-ps4-%s", (long)now_seconds() + g_cfg.token_ttl,
           (long)now_seconds() - 60, user_id);
  iso8601_now(now, sizeof(now));
  // ias is read with sub_5fa7e0 and 0 sends the client to SignupRequired (sub_c74414); Peacock
  // sends 2.
  http_res_json(res, 200,
                json_pack("{s:s, s:s, s:i, s:{s:s, s:s, s:f}, s:s, s:s}", "token", token, "profileid", user_id,
                          "ias", g_cfg.ias, "blobconfig", "bloburl", bloburl, "blobsig",
                          "?sv=2018-03-28&ver=h3", "blobsigduration", 7200000.0, "serverversion", g_cfg.game_ver,
                          "servertimeutc", now));
}

// ------------------------------------------------------------------ 4.6 ProfileService

static void rpc_get_profile(http_req_t *req, http_res_t *res, const char *user_id) {
  json_t *body = json_body(req, res);
  if (!body) return;
  json_t *names = json_object_get(body, "extensions");
  if (names && !json_is_array(names)) names = NULL;
  const char *id = jstr(body, "id");
  if (id && strcmp(id, user_id)) LOGW("GetProfile asked for %s by profile %s: answering with the caller's profile", id, user_id);
  json_t *profile = db_get_profile(user_id, names);
  if (profile) http_res_json(res, 200, profile);
  else http_res_error(res, 404, "profile_not_found", user_id);
  json_decref(body);
}

// ResolveProfiles: returns a UserProfile for each requested id (network map ProfileService list;
// matches Peacock). Only the caller's own profile is known here; others resolve to a minimal stub
// so the client's friends/leaderboard lookups do not error.
static void rpc_resolve_profiles(http_req_t *req, http_res_t *res, const char *user_id) {
  json_t *body = json_body(req, res);
  if (!body) return;
  json_t *ids = json_object_get(body, "profileIDs");
  if (!ids) ids = json_object_get(body, "profileIds");
  json_t *out = json_array();
  size_t i;
  json_t *v;
  if (json_is_array(ids)) {
    json_array_foreach(ids, i, v) {
      const char *id = json_is_string(v) ? json_string_value(v) : NULL;
      if (!id || strlen(id) != HS_ID_LEN) continue;
      if (!strcmp(id, user_id)) {
        json_t *none = json_array();  // identity only, no extensions
        json_t *p = db_get_profile(user_id, none);
        json_decref(none);
        if (p) json_array_append_new(out, p);
      } else {
        json_array_append_new(out, json_pack("{s:s, s:{}, s:{}, s:n, s:n}", "Id", id, "LinkedAccounts", "Extensions",
                                             "ETag", "Gamertag"));
      }
    }
  }
  LOGI("ResolveProfiles: %zu id(s) -> %zu profile(s)", json_is_array(ids) ? json_array_size(ids) : 0,
       json_array_size(out));
  http_res_json(res, 200, out);
  json_decref(body);
}

static void rpc_resolve_gamertags(http_req_t *req, http_res_t *res, const char *user_id) {
  json_t *body = json_body(req, res);
  if (!body) return;
  json_t *ids = json_object_get(body, "profileIds");
  if (!ids) ids = json_object_get(body, "profileIDs");
  // Peacock groups tags by platform; the PS5 identity is a "ps4" account.
  json_t *ps4 = json_object();
  size_t i;
  json_t *v;
  if (json_is_array(ids)) {
    json_array_foreach(ids, i, v) {
      const char *id = json_is_string(v) ? json_string_value(v) : NULL;
      if (!id || strcmp(id, user_id)) continue;
      json_t *none = json_array();
      json_t *p = db_get_profile(user_id, none);
      json_decref(none);
      if (p) {
        const char *tag = json_string_value(json_object_get(p, "Gamertag"));
        if (tag) json_object_set_new(ps4, id, json_string(tag));
        json_decref(p);
      }
    }
  }
  http_res_json(res, 200, json_pack("{s:o, s:{}, s:{}}", "ps4", ps4, "steam", "dev"));
  json_decref(body);
}

static void rpc_update_extensions(http_req_t *req, http_res_t *res, const char *user_id) {
  json_t *body = json_body(req, res);
  if (!body) return;
  json_t *data = json_object_get(body, "extensionsData");
  if (!json_is_object(data)) {
    http_res_error(res, 400, "invalid_request", "extensionsData must be an object");
    json_decref(body);
    return;
  }
  char names[512] = "";
  size_t o = 0;
  const char *name;
  json_t *value;
  json_object_foreach(data, name, value) {
    size_t n = strlen(name);
    if (n == 0 || n > 128) {
      http_res_error(res, 400, "invalid_request", "extension names must be 1-128 characters");
      json_decref(body);
      return;
    }
    if (o < sizeof(names)) o += (size_t)snprintf(names + o, sizeof(names) - o, "%s%s", o ? "," : "", name);
  }
  if (db_update_extensions(user_id, data)) {
    http_res_error(res, 500, "server_error", NULL);
  } else {
    LOGI("profile %s: stored extensions %s", user_id, names);
    http_res_json(res, 200, json_incref(data));
  }
  json_decref(body);
}

// ------------------------------------------------------------------ 4.7 EventsService

static void rpc_events(http_req_t *req, http_res_t *res, const char *user_id) {
  json_t *body = json_body(req, res);
  if (!body) return;
  json_t *values = json_object_get(body, "values");
  if (values && !json_is_array(values)) {
    http_res_error(res, 400, "invalid_request", "values must be an array");
    json_decref(body);
    return;
  }
  size_t n = values ? json_array_size(values) : 0;

  // clienttype is the PS5 addition over the PC request (sub_c6e5ee).
  json_t *ct = json_object_get(body, "clienttype");
  char clienttype[64] = "-";
  if (json_is_string(ct)) snprintf(clienttype, sizeof(clienttype), "%s", json_string_value(ct));
  else if (json_is_integer(ct)) snprintf(clienttype, sizeof(clienttype), "%lld", (long long)json_integer_value(ct));
  const char *body_user = jstr(body, "userId");
  if (body_user && strcmp(body_user, user_id)) {
    // Upstream answers 403 here too (eventHandler.ts): events are never filed under someone else.
    LOGW("events: body userId %s is not the caller's profile %s, refused", body_user, user_id);
    http_res_error(res, 403, "forbidden", "userId is not the caller's profile");
    json_decref(body);
    return;
  }
  json_t *last_ticks = json_object_get(body, "lastEventTicks");
  json_t *last_push = json_object_get(body, "lastPushDt");

  hs_event_t *events = calloc(n ? n : 1, sizeof(*events));
  int64_t *ticks = calloc(n ? n : 1, sizeof(*ticks));
  size_t m = 0;
  if (!events || !ticks) {
    http_res_error(res, 500, "server_error", NULL);
    goto done;
  }
  for (size_t i = 0; i < n; i++) {
    json_t *ev = json_array_get(values, i);
    if (!json_is_object(ev)) {
      LOGW("events: values[%zu] is not an object, skipped", i);
      continue;
    }
    hs_event_t *e = &events[m++];
    e->id = jstr(ev, "Id");
    e->name = json_is_string(json_object_get(ev, "Name")) ? json_string_value(json_object_get(ev, "Name")) : "";
    e->contract_session_id = jstr(ev, "ContractSessionId");
    e->contract_id = jstr(ev, "ContractId");
    e->origin = jstr(ev, "Origin");
    json_t *ts = json_object_get(ev, "Timestamp");
    if ((e->has_timestamp = json_is_number(ts))) e->timestamp = json_number_value(ts);
    json_t *xp = json_object_get(ev, "XPGain");
    if ((e->has_xp_gain = json_is_number(xp))) e->xp_gain = (int64_t)json_number_value(xp);
    json_t *val = json_object_get(ev, "Value");
    e->value_json = val ? json_dumps(val, JSON_COMPACT | JSON_ENCODE_ANY) : NULL;
    LOGD("event %s session=%s contract=%s", e->name, e->contract_session_id ? e->contract_session_id : "-",
         e->contract_id ? e->contract_id : "-");
  }
  if (db_save_events(user_id, events, m, ticks)) {
    http_res_error(res, 500, "server_error", NULL);
    goto done;
  }
  LOGI("events: profile %s clienttype=%s saved=%zu lastEventTicks=%.0f lastPushDt=%.3f", user_id, clienttype, m,
       json_is_number(last_ticks) ? json_number_value(last_ticks) : -1.0,
       json_is_number(last_push) ? json_number_value(last_push) : -1.0);

  json_t *saved = json_null();
  if (m > 0) {
    json_decref(saved);
    saved = json_array();
    for (size_t i = 0; i < m; i++) {
      char t[32];
      snprintf(t, sizeof(t), "%lld", (long long)ticks[i]);
      json_array_append_new(saved, json_string(t));
    }
  }
  http_res_json(res, 200,
                json_pack("{s:o, s:n, s:f, s:n}", "SavedTokens", saved, "NewEvents", "NextPoll", g_cfg.next_poll,
                          "PushMessages"));
done:
  for (size_t i = 0; i < m; i++) free(events[i].value_json);
  free(events);
  free(ticks);
  json_decref(body);
}

// Standalone mode keeps no mission saves: the shape is checked and the request acknowledged the
// way upstream does (204), and anything that would have been saved is reported in the log. Proxy
// mode stores them in Peacock (profileHandler.ts UpdateUserSaveFileTable).
static void rpc_save_file_table(http_req_t *req, http_res_t *res, const char *user_id) {
  json_t *body = json_body(req, res);
  if (!body) return;
  json_t *saves = json_object_get(body, "clientSaveFileList");
  json_t *deleted = json_object_get(body, "deletedSaveFileList");
  if ((saves && !json_is_array(saves)) || (deleted && !json_is_array(deleted))) {
    http_res_error(res, 400, "invalid_request", "clientSaveFileList and deletedSaveFileList must be arrays");
  } else {
    size_t ns = saves ? json_array_size(saves) : 0, nd = deleted ? json_array_size(deleted) : 0;
    if (ns || nd) LOGW("profile %s: %zu save(s), %zu deletion(s) not kept: standalone mode has no save storage", user_id, ns, nd);
    http_res_empty(res, 204);
  }
  json_decref(body);
}

// ------------------------------------------------------------------ userchannel dispatch

static void h_rpc(http_req_t *req, http_res_t *res, const char *service, const char *method) {
  if (!method_is(req, res, "POST")) return;
  char user_id[HS_ID_LEN + 1];
  if (!require_user(req, res, user_id)) return;
  const char *version = http_header(req, "Version");
  LOGD("rpc %s/%s profile=%s version=%s", service, method, user_id, version ? version : "-");

  if (str_ieq(service, "ProfileService")) {
    if (str_ieq(method, "GetProfile")) { rpc_get_profile(req, res, user_id); return; }
    if (str_ieq(method, "UpdateExtensions")) { rpc_update_extensions(req, res, user_id); return; }
    if (str_ieq(method, "ResolveProfiles")) { rpc_resolve_profiles(req, res, user_id); return; }
    if (str_ieq(method, "ResolveGamerTags")) { rpc_resolve_gamertags(req, res, user_id); return; }
    // Peacock: GetFriendsCount -> "0", 204 for sync, {} for GetUserConfig, "null" for entitlements.
    if (str_ieq(method, "GetFriendsCount")) { http_res_json(res, 200, json_integer(0)); return; }
    if (str_ieq(method, "SynchronizeOfflineUnlockables")) { http_res_empty(res, 204); return; }
    if (str_ieq(method, "GetUserConfig")) { http_res_json(res, 200, json_object()); return; }
    if (str_ieq(method, "SetClientEntitlements")) { http_res_json(res, 200, json_string("null")); return; }
    if (str_ieq(method, "UpdateUserSaveFileTable")) { rpc_save_file_table(req, res, user_id); return; }
    if (str_ieq(method, "UpdateProfileStats")) { http_res_empty(res, 204); return; }
    if (str_ieq(method, "GetPlatformEntitlements")) { http_res_json(res, 200, json_array()); return; }
  } else if (str_ieq(service, "EventsService")) {
    if (str_ieq(method, "SaveAndSynchronizeEvents4")) { rpc_events(req, res, user_id); return; }
  } else if (str_ieq(service, "AuthenticationService")) {
    // Flow steps 21-22: the menu files the client should take from the server instead of its
    // offline cache. Standalone mode serves no menu files, so it lists none (upstream answers
    // with the files its menu system really serves).
    if (str_ieq(method, "GetBlobOfflineCacheDatabaseDiff")) {
      http_res_json(res, 200, json_array());
      return;
    }
    if (str_ieq(method, "RenewBlobSignature")) {
      char base[300], bloburl[400];
      base_url(req, base, sizeof(base));
      snprintf(bloburl, sizeof(bloburl), "%s/resources-8-25/", base);
      http_res_json(res, 200, json_pack("{s:s, s:s, s:f}", "bloburl", bloburl, "blobsig", "?sv=2018-03-28&ver=h3",
                                        "blobsigduration", 7200000.0));
      return;
    }
  }
  not_found(req, res);
}

// ------------------------------------------------------------------ onlineconfig.json

// The Online.* tunables the game fetches from Configuration.Url after the config (network map 5.4;
// matches Peacock's static/onlineconfig.json). WebSocketsEnabled is false so everything stays on
// HTTP polling.
static void h_onlineconfig(http_req_t *req, http_res_t *res) {
  if (!method_is(req, res, "GET")) return;
  json_t *o = json_pack(
      "{s:{s:{s:f, s:f, s:{s:b, s:b, s:b, s:b, s:b}}}, "
      "s:{s:f, s:f, s:f, s:f, s:f, s:s, s:f, s:b, s:{s:f, s:f, s:b}}}",
      "Contracts",
        "Events", "PollingTime", 20.0, "PollingRandomTime", 2.0,
          "Enabled", "ContractBegins", 1, "TargetKilled", 1, "CorrectWeaponUsed", 1, "CorrectDisguiseUsed", 1,
                     "CorrectLocationUsed", 1,
      "Online",
        "BlobSignatureRenewalMinutes", 5.0, "BlobSignatureRenewalCooldownSeconds", 30.0,
        "GPSTokenRenewalMinutes", 5.0, "GPSTokenRenewalCooldownSeconds", 30.0,
        "DefaultHttpTimeout", 30.0, "WebSocketsRoute", "authentication/ws", "DefaultWebSocketTimeout", 30.0,
        "WebSocketsEnabled", 0,
        "DefaultAdaptiveHttpTimeout", "timeout_s", 99.0, "speed_kbs", 50.0, "enabled", 1);
  LOGI("onlineconfig served to %s", req->peer);
  http_res_json(res, 200, o);
}

// The GDPR consent dialog from Configuration.AgreementUrl. The client HEADs it first to read the
// x-ms-meta-version header, then GETs it (Peacock: "it silently fails on some game versions without
// this [header], no clue why"). Content-Type must be application/octet-stream.
static void h_privacy(http_req_t *req, http_res_t *res) {
  if (!method_is(req, res, "GET")) return;
  snprintf(res->extra_headers, sizeof(res->extra_headers), "x-ms-meta-version: 20181001\r\n");
  // The GDPR consent modal, with the checkbox + Accept + Decline buttons the client renders
  // (matches Peacock's static/PrivacyPolicy.json). Without the buttons the game shows the policy
  // with no Accept and treats it as declined, staying offline.
  json_t *o = json_pack(
      "{s:s, s:s, s:s, s:[{s:s, s:s, s:s}, {s:s, s:s}, {s:s, s:s}], "
      "s:{s:s, s:i, s:i, s:s, s:[{s:s}, {s:s}]}}",
      "view", "menu3.modal.ModalDialogGeneric", "priority", "gdprconsent", "type", "gdprconsent",
      "buttons",
        "label", "$loc UI_DIALOG_ONLINE_GDPR_CHECK_DESCRIPTION", "type", "checkbox", "icon", "check",
        "label", "$loc UI_DIALOG_ACCEPT", "type", "ok",
        "label", "$loc UI_DIALOG_DECLINE", "type", "cancel",
      "data",
        "title", "$loc UI_DIALOG_ONLINE_GDPR_TITLE", "frameheightmax", 600, "dialogWidth", 1250, "typeoverride",
        "online", "information",
          "description", "This is a local HITMAN preservation server running on this console. Your game and profile "
                         "data are stored only on this console.",
          "description", "By continuing you agree to use this local server for online features.");
  res->content_type = "application/octet-stream";
  res->body = json_dumps(o, JSON_COMPACT);
  res->body_len = res->body ? strlen(res->body) : 0;
  res->status = 200;
  json_decref(o);
  LOGI("privacy policy (GDPR consent) served to %s", req->peer);
}

// ------------------------------------------------------------------ resources, metrics, admin

static void h_resource(http_req_t *req, http_res_t *res, const char *rel) {
  if (!method_is(req, res, "GET")) return;
  if (!*rel || strstr(rel, "..") || rel[0] == '/') {
    http_res_error(res, 400, "invalid_request", "bad resource path");
    return;
  }
  // The PS5 build asks for dynamic_resources_ps5_release_rpkg (dynamic_resources_{0}_{1}_rpkg,
  // sub_c7bb7c). Upstream ships one H3 package for it; the console mounted that file on
  // 2026-10-15 (online state 24). Only that exact name is mapped: any other platform or
  // version name is a 404 rather than a guess.
  if (str_ieq(rel, "dynamic_resources_ps5_release_rpkg")) rel = "dynamic_resources_h3.rpkg";

  char path[1024];
  snprintf(path, sizeof(path), "%s/%s", g_cfg.resources_dir, rel);
  struct stat st;
  if (stat(path, &st) || !S_ISREG(st.st_mode)) {
    LOGW("resource not found: %s (put it at %s)", rel, path);
    http_res_error(res, 404, "not_found", rel);
    return;
  }
  res->status = 200;
  res->content_type = "application/octet-stream";
  snprintf(res->file_path, sizeof(res->file_path), "%s", path);
  res->file_size = st.st_size;
  LOGI("resource served: %s (%lld bytes)", rel, (long long)st.st_size);
}

static void *shutdown_later(void *arg) {
  (void)arg;
  usleep(300 * 1000);
  LOGI("shutting down on request");
  http_stop();
  db_close();
  _exit(0);
  return NULL;
}

static void h_shutdown(http_req_t *req, http_res_t *res) {
  if (!method_is(req, res, "POST")) return;
  if (!req->peer_loopback) {
    http_res_error(res, 403, "forbidden", "only accepted from the console itself");
    return;
  }
  pthread_t th;
  if (pthread_create(&th, NULL, shutdown_later, NULL) == 0) pthread_detach(th);
  res->close = true;
  http_res_json(res, 200, json_pack("{s:b}", "ok", 1));
}

// ------------------------------------------------------------------ router

static void h_healthz(http_req_t *req, http_res_t *res) {
  char patcher[256], upstream[512];
  patcher_status(patcher, sizeof(patcher));
  if (g_cfg.upstream[0]) proxy_status(upstream, sizeof(upstream));
  else snprintf(upstream, sizeof(upstream), "none: standalone mode, built-in handlers");
  http_res_json(res, 200, json_pack("{s:b, s:s, s:s, s:s, s:s, s:s}", "ok", 1, "name", HS_NAME, "version", HS_VERSION,
                                    "mode", g_cfg.upstream[0] ? "proxy" : "standalone", "upstream", upstream,
                                    "patcher", g_cfg.patch_enabled ? patcher : "disabled in config.ini"));
}

void app_handle(http_req_t *req, http_res_t *res) {
  const char *p = req->path;

  // Served here in both modes: status and the loopback-only handoff used by new builds.
  if (str_ieq(p, "/healthz")) { h_healthz(req, res); return; }
  if (str_ieq(p, "/admin/shutdown")) { h_shutdown(req, res); return; }

  // Unless lan_access = full, the LAN only gets /healthz: game routes are for the console.
  if (!req->peer_loopback && !g_cfg.lan_full) {
    LOGW("refused %s %s from %s (lan_access = health)", req->is_head ? "HEAD" : req->method, p, req->peer);
    http_res_error(res, 403, "forbidden", "only /healthz is served to the LAN (lan_access = health)");
    return;
  }

  if (g_cfg.upstream[0]) {
    proxy_forward(req, res);
    return;
  }

  // The PS5 client fetches the root (GET / with Accept: application/json, UA hitman6/.../sceCURL)
  // at BEGIN_CONNECTION_FLOW, before the config URL. The shape it wants here is not in the network
  // map; serve the same config body as /config/ps4-prod/{game_ver} and log what it does next.
  if (str_ieq(p, "/") && !strcmp(req->method, "GET")) {
    LOGI("root probe from %s (ua %s), answering with the ps4-prod config", req->peer,
         http_header(req, "User-Agent") ? http_header(req, "User-Agent") : "-");
    { h_config(req, res, "ps4-prod", g_cfg.game_ver); return; }
  }

  if (str_iprefix(p, "/config/")) {
    char name[80], version[80];
    const char *rest = p + 8;
    const char *slash = strchr(rest, '/');
    if (slash && !strchr(slash + 1, '/') && (size_t)(slash - rest) < sizeof(name) && strlen(slash + 1) < sizeof(version)) {
      snprintf(name, sizeof(name), "%.*s", (int)(slash - rest), rest);
      snprintf(version, sizeof(version), "%s", slash + 1);
      { h_config(req, res, name, version); return; }
    }
  } else if (str_ieq(p, PATH_OAUTH)) {
    { h_oauth(req, res); return; }
  } else if (str_ieq(p, "/" PATH_CONFIGURATION "/Init")) {
    { h_init(req, res); return; }
  } else if (str_iprefix(p, "/" PATH_USERCHANNEL "/")) {
    const char *rest = p + strlen("/" PATH_USERCHANNEL "/");
    const char *slash = strchr(rest, '/');
    char service[80];
    if (slash && !strchr(slash + 1, '/') && (size_t)(slash - rest) < sizeof(service)) {
      snprintf(service, sizeof(service), "%.*s", (int)(slash - rest), rest);
      { h_rpc(req, res, service, slash + 1); return; }
    }
  } else if (str_ieq(p, "/files/onlineconfig.json")) {
    { h_onlineconfig(req, res); return; }
  } else if (str_iprefix(p, "/files/privacypolicy/")) {
    { h_privacy(req, res); return; }
  } else if (str_ieq(p, "/" PATH_METRICS) || str_iprefix(p, "/api/metrics/")) {
    LOGD("metrics: %zu bytes dropped", req->body_len);
    { http_res_empty(res, 204); return; }
  } else if (str_iprefix(p, "/" PATH_RESOURCES "/")) {
    { h_resource(req, res, p + strlen("/" PATH_RESOURCES "/")); return; }
  } else if (str_iprefix(p, "/files/")) {
    { h_resource(req, res, p + strlen("/files/")); return; }
  } else if (str_iprefix(p, "/resources-")) {
    // bloburl is /resources-{version}/..., where the client appends rpkg names.
    const char *slash = strchr(p + 1, '/');
    { h_resource(req, res, slash ? slash + 1 : ""); return; }
  }
  not_found(req, res);
}
