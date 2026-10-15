// config.ini loader: "key = value" lines, '#' comments. Unknown keys are reported, not fatal.
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hs.h"

hs_config_t g_cfg;

static void defaults(void) {
  memset(&g_cfg, 0, sizeof(g_cfg));
  snprintf(g_cfg.bind, sizeof(g_cfg.bind), "0.0.0.0");
  g_cfg.port = 8000;
  snprintf(g_cfg.data_dir, sizeof(g_cfg.data_dir), "%s", HS_DEFAULT_DATA);
  snprintf(g_cfg.game_ver, sizeof(g_cfg.game_ver), "8.12.0");
  g_cfg.token_ttl = 3600;
  g_cfg.refresh_ttl = 30 * 24 * 3600;
  g_cfg.ias = 2;  // Peacock sends 2; 0 would send the client to SignupRequired
  g_cfg.next_poll = 10.0;
  g_cfg.log_level = LOG_INFO;
  g_cfg.patch_enabled = true;
  snprintf(g_cfg.title_id, sizeof(g_cfg.title_id), "PPSA16387");
  g_cfg.psn_bypass = true;
  snprintf(g_cfg.psn_country, sizeof(g_cfg.psn_country), "gb");
  g_cfg.upstream_timeout = 30;
}

static char *trim(char *s) {
  while (isspace((unsigned char)*s)) s++;
  char *e = s + strlen(s);
  while (e > s && isspace((unsigned char)e[-1])) *--e = '\0';
  return s;
}

static int parse_int(const char *key, const char *v, int *out, int min, int max) {
  char *end;
  errno = 0;
  long n = strtol(v, &end, 10);
  if (errno || *end || end == v || n < min || n > max) {
    fprintf(stderr, "config: %s must be an integer in [%d, %d], got \"%s\"\n", key, min, max, v);
    return -1;
  }
  *out = (int)n;
  return 0;
}

static int parse_bool(const char *key, const char *v, bool *out) {
  if (!strcmp(v, "1") || str_ieq(v, "true") || str_ieq(v, "yes") || str_ieq(v, "on")) { *out = true; return 0; }
  if (!strcmp(v, "0") || str_ieq(v, "false") || str_ieq(v, "no") || str_ieq(v, "off")) { *out = false; return 0; }
  fprintf(stderr, "config: %s must be true or false, got \"%s\"\n", key, v);
  return -1;
}

#define COPY(field) snprintf(g_cfg.field, sizeof(g_cfg.field), "%s", v)

static int apply(const char *k, const char *v) {
  if (!strcmp(k, "bind")) COPY(bind);
  else if (!strcmp(k, "port")) return parse_int(k, v, &g_cfg.port, 0, 65535);
  else if (!strcmp(k, "public_url")) COPY(public_url);
  else if (!strcmp(k, "data_dir")) COPY(data_dir);
  else if (!strcmp(k, "db_path")) COPY(db_path);
  else if (!strcmp(k, "log_path")) COPY(log_path);
  else if (!strcmp(k, "resources_dir")) COPY(resources_dir);
  else if (!strcmp(k, "seed_extensions")) COPY(seed_extensions);
  else if (!strcmp(k, "game_ver")) COPY(game_ver);
  else if (!strcmp(k, "rpc_version")) COPY(rpc_version);
  else if (!strcmp(k, "token_ttl")) return parse_int(k, v, &g_cfg.token_ttl, 60, 30 * 24 * 3600);
  else if (!strcmp(k, "refresh_ttl")) return parse_int(k, v, &g_cfg.refresh_ttl, 0, 365 * 24 * 3600);
  else if (!strcmp(k, "ias")) return parse_int(k, v, &g_cfg.ias, 1, 1 << 30);
  else if (!strcmp(k, "next_poll")) {
    char *end;
    g_cfg.next_poll = strtod(v, &end);
    if (*end || end == v || g_cfg.next_poll < 1) {
      fprintf(stderr, "config: next_poll must be a number >= 1, got \"%s\"\n", v);
      return -1;
    }
  } else if (!strcmp(k, "log_level")) {
    if (str_ieq(v, "debug")) g_cfg.log_level = LOG_DEBUG;
    else if (str_ieq(v, "info")) g_cfg.log_level = LOG_INFO;
    else if (str_ieq(v, "warn")) g_cfg.log_level = LOG_WARN;
    else if (str_ieq(v, "error")) g_cfg.log_level = LOG_ERROR;
    else { fprintf(stderr, "config: log_level must be debug|info|warn|error\n"); return -1; }
  } else if (!strcmp(k, "patch")) return parse_bool(k, v, &g_cfg.patch_enabled);
  else if (!strcmp(k, "title_id")) COPY(title_id);
  else if (!strcmp(k, "patch_domain")) COPY(patch_domain);
  else if (!strcmp(k, "psn_bypass")) return parse_bool(k, v, &g_cfg.psn_bypass);
  else if (!strcmp(k, "psn_online_id")) COPY(psn_online_id);
  else if (!strcmp(k, "psn_account_id")) COPY(psn_account_id);
  else if (!strcmp(k, "psn_country")) COPY(psn_country);
  else if (!strcmp(k, "upstream")) COPY(upstream);
  else if (!strcmp(k, "upstream_timeout")) return parse_int(k, v, &g_cfg.upstream_timeout, 2, 600);
  else if (!strcmp(k, "lan_access")) {
    if (str_ieq(v, "health")) g_cfg.lan_full = false;
    else if (str_ieq(v, "full")) g_cfg.lan_full = true;
    else { fprintf(stderr, "config: lan_access must be health or full\n"); return -1; }
  }
  else fprintf(stderr, "config: ignoring unknown key \"%s\"\n", k);
  return 0;
}

static bool valid_game_ver(const char *v) {
  // sub_c56280 keeps the Versions[] entry whose GAME_VER major equals the client's (8).
  unsigned a, b, c;
  char extra;
  return sscanf(v, "%u.%u.%u%c", &a, &b, &c, &extra) == 3 && a == 8;
}

// upstream = http://<host>:<port>[/]. The console has no TLS client here and payload DNS is not
// dependable, so the host must be an IPv4 literal on the PS5; host builds also accept names.
static int parse_upstream(void) {
  size_t n = strlen(g_cfg.upstream);
  while (n > 0 && g_cfg.upstream[n - 1] == '/') g_cfg.upstream[--n] = '\0';
  if (!n) return 0;
  if (strncmp(g_cfg.upstream, "http://", 7)) {
    fprintf(stderr, "config: upstream must be http://<host>:<port> (no https on the console)\n");
    return -1;
  }
  const char *h = g_cfg.upstream + 7;
  const char *colon = strrchr(h, ':');
  if (strchr(h, '/') || !colon || colon == h || (size_t)(colon - h) >= sizeof(g_cfg.upstream_host)) {
    fprintf(stderr, "config: upstream must be http://<host>:<port> with no path, got \"%s\"\n", g_cfg.upstream);
    return -1;
  }
  snprintf(g_cfg.upstream_host, sizeof(g_cfg.upstream_host), "%.*s", (int)(colon - h), h);
  if (parse_int("upstream port", colon + 1, &g_cfg.upstream_port, 1, 65535)) return -1;
#ifdef __PROSPERO__
  unsigned a, b, c, d;
  char extra;
  if (sscanf(g_cfg.upstream_host, "%u.%u.%u.%u%c", &a, &b, &c, &d, &extra) != 4 || a > 255 || b > 255 || c > 255 ||
      d > 255) {
    fprintf(stderr, "config: upstream host must be an IPv4 address on the console, got \"%s\"\n", g_cfg.upstream_host);
    return -1;
  }
#endif
  // Forwarding to ourselves would loop until the connection limit is hit.
  bool self = !strcmp(g_cfg.upstream_host, "localhost") || !strncmp(g_cfg.upstream_host, "127.", 4) ||
              !strcmp(g_cfg.upstream_host, "0.0.0.0") || !strcmp(g_cfg.upstream_host, g_cfg.bind);
  if (self && g_cfg.upstream_port == g_cfg.port) {
    fprintf(stderr, "config: upstream %s is this server's own address\n", g_cfg.upstream);
    return -1;
  }
  return 0;
}

// Returns 0 on success. A missing file is fine (defaults are used).
int config_load(const char *path) {
  defaults();
  FILE *f = fopen(path, "r");
  int rc = 0;
  if (f) {
    char line[1024];
    int lineno = 0;
    while (fgets(line, sizeof(line), f)) {
      lineno++;
      char *s = trim(line);
      if (!*s || *s == '#' || *s == ';') continue;
      char *eq = strchr(s, '=');
      if (!eq) {
        fprintf(stderr, "config: %s:%d: expected key = value\n", path, lineno);
        rc = -1;
        continue;
      }
      *eq = '\0';
      if (apply(trim(s), trim(eq + 1))) rc = -1;
    }
    fclose(f);
  }

  if (!g_cfg.db_path[0]) snprintf(g_cfg.db_path, sizeof(g_cfg.db_path), "%s/hitman.db", g_cfg.data_dir);
  if (!g_cfg.log_path[0]) snprintf(g_cfg.log_path, sizeof(g_cfg.log_path), "%s/server.log", g_cfg.data_dir);
  if (!g_cfg.resources_dir[0]) snprintf(g_cfg.resources_dir, sizeof(g_cfg.resources_dir), "%s/resources", g_cfg.data_dir);
  if (!g_cfg.rpc_version[0]) snprintf(g_cfg.rpc_version, sizeof(g_cfg.rpc_version), "%s", g_cfg.game_ver);
  if (!g_cfg.patch_domain[0]) snprintf(g_cfg.patch_domain, sizeof(g_cfg.patch_domain), "127.0.0.1:%d", g_cfg.port);
  size_t pu = strlen(g_cfg.public_url);
  while (pu > 0 && g_cfg.public_url[pu - 1] == '/') g_cfg.public_url[--pu] = '\0';

  if (!valid_game_ver(g_cfg.game_ver)) {
    fprintf(stderr, "config: game_ver must be 8.x.y\n");
    rc = -1;
  }
  if (g_cfg.public_url[0] && strncmp(g_cfg.public_url, "http://", 7) && strncmp(g_cfg.public_url, "https://", 8)) {
    fprintf(stderr, "config: public_url must start with http:// or https://\n");
    rc = -1;
  }
  // "http://" + domain replaces the 23-byte "https://auth.hitman.io" in the game.
  if (parse_upstream()) rc = -1;
  if (strlen(g_cfg.patch_domain) > 15) {
    fprintf(stderr, "config: patch_domain must be at most 15 characters\n");
    rc = -1;
  }
  return rc;
}
