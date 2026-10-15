// HITMAN 3 (PS5) local online service. Shared declarations.
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include <jansson.h>

#define HS_NAME    "hitman-server"
#define HS_VERSION "0.3.0"

#ifdef __PROSPERO__
#define HS_DEFAULT_CONFIG "/data/hitman-server/config.ini"
#define HS_DEFAULT_DATA   "/data/hitman-server"
#else
#define HS_DEFAULT_CONFIG "config.ini"
#define HS_DEFAULT_DATA   "data"
#endif

#define HS_ID_LEN    36  // uuid
#define HS_TOKEN_LEN 64  // 32 random bytes, hex

// ---------------------------------------------------------------- config.c
typedef struct {
  char   bind[64];
  int    port;
  char   public_url[256];   // empty: derive from the request Host header
  char   data_dir[256];
  char   db_path[512];
  char   log_path[512];
  char   resources_dir[512];
  char   seed_extensions[512];
  char   game_ver[32];      // Versions[].GAME_VER and Init.serverversion
  char   rpc_version[32];   // Versions[].Version (RPC "Version" header source, inferred)
  int    token_ttl;         // OAuth expires_in, seconds
  int    refresh_ttl;       // how long a refresh token outlives its access token, seconds
  int    ias;               // Init.ias, must be non-zero
  double next_poll;         // SaveAndSynchronizeEvents4.NextPoll, seconds
  int    log_level;
  bool   patch_enabled;     // in-memory URL patch of the running game (PS5 only)
  char   title_id[16];
  char   patch_domain[64];  // empty: 127.0.0.1:<port>
  bool   psn_bypass;        // make the game's PSN checks succeed (PS5 only)
  char   psn_online_id[17]; // fallback online id; empty: the PS5 user name
  char   psn_account_id[24];// fallback account id (decimal); empty: derived from the online id
  char   psn_country[4];    // fallback account country
  char   dlc_titles[256];   // other title ids whose installed add-ons the game is told about
  // Proxy mode (docs/ARCHITECTURE_PS5_PEACOCK.md): when upstream is set, every game request is
  // forwarded to the full Peacock server there and the built-in handlers are not used.
  char   upstream[256];     // "http://<ipv4>:<port>"; empty: standalone (legacy) mode
  char   upstream_host[64]; // parsed from upstream
  int    upstream_port;
  int    upstream_timeout;  // seconds for one upstream exchange
  bool   lan_full;          // lan_access = full: LAN peers may use every route, not just /healthz
} hs_config_t;

extern hs_config_t g_cfg;

int config_load(const char *path);

// ---------------------------------------------------------------- log.c
enum { LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR };

void log_open(const char *path, int level);
void log_msg(int level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
void notify(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#define LOGD(...) log_msg(LOG_DEBUG, __VA_ARGS__)
#define LOGI(...) log_msg(LOG_INFO, __VA_ARGS__)
#define LOGW(...) log_msg(LOG_WARN, __VA_ARGS__)
#define LOGE(...) log_msg(LOG_ERROR, __VA_ARGS__)

// ---------------------------------------------------------------- util.c
void    random_hex(char *out, size_t nbytes);  // writes 2*nbytes hex chars + NUL
void    uuid_v4(char out[HS_ID_LEN + 1]);
int64_t now_seconds(void);
int64_t now_micros(void);
void    iso8601_now(char *out, size_t n);       // 2026-10-15T03:00:00.000Z
bool    str_ieq(const char *a, const char *b);
bool    str_iprefix(const char *s, const char *prefix);
int     mkdirs(const char *path);

// ---------------------------------------------------------------- http.c
#define HTTP_MAX_HEADERS 64

typedef struct {
  const char *name;
  const char *value;
} http_header_t;

typedef struct {
  char          method[16];
  char          path[1024];     // percent-decoded, duplicate slashes collapsed, no trailing slash
  const char   *query;          // raw query string without '?', never NULL
  http_header_t headers[HTTP_MAX_HEADERS];
  int           nheaders;
  char         *body;           // NUL-terminated, may contain NULs before body_len
  size_t        body_len;
  char          peer[64];
  bool          peer_loopback;
  bool          is_head;        // the client sent HEAD (method is rewritten to GET for routing)
  char          target[2048];   // raw request target as sent (origin-form), for forwarding
} http_req_t;

typedef struct {
  int         status;
  const char *content_type;
  char       *body;             // malloc'd, owned
  size_t      body_len;
  char        file_path[1024];  // when set, the file is streamed instead of body
  off_t       file_size;
  char        extra_headers[512];  // extra response header lines, each ending in CRLF
  char       *more_headers;     // malloc'd extra header lines (forwarded upstream headers), owned
  long long   head_length;      // Content-Length to report for HEAD when >= 0 (proxied HEAD)
  bool        close;            // close the connection after this response
} http_res_t;

typedef void (*http_handler_t)(http_req_t *req, http_res_t *res);

const char *http_header(const http_req_t *req, const char *name);
// Looks up key in an application/x-www-form-urlencoded string. Returns false when absent.
bool http_form_get(const char *form, const char *key, char *out, size_t outlen);
void http_res_json(http_res_t *res, int status, json_t *body);  // steals body
void http_res_empty(http_res_t *res, int status);
void http_res_error(http_res_t *res, int status, const char *error, const char *description);
int  http_listen(const char *bind_addr, int port);  // 0, or an errno value
void http_run(http_handler_t handler);                // accept loop, returns after http_stop()
void http_stop(void);
// The query string with the values of secret-looking keys replaced, for logging.
void http_redact_query(const char *query, char *out, size_t n);
// Form body (key=value&...) with every value outside a small allowlist replaced, for logging.
void http_redact_form(const char *form, char *out, size_t n);

// ---------------------------------------------------------------- proxy.c
// Forwards req to g_cfg.upstream and fills res with the upstream response (status, headers and
// body byte for byte). Upstream failures become a 502/504 JSON error.
void proxy_forward(http_req_t *req, http_res_t *res);
// One line for /healthz: whether the upstream answers and how fast.
void proxy_status(char *out, size_t n);
// The access token the game received from its last successful /oauth/token, attached to game
// requests that come without one (the client only sends Authorization over https, sub_c43dee).
bool proxy_session_token(char *out, size_t n);
void proxy_remember_token(const char *access_token);

// ---------------------------------------------------------------- db.c
typedef struct {
  const char *id;
  const char *name;
  const char *contract_session_id;
  const char *contract_id;
  bool        has_timestamp;
  double      timestamp;
  const char *origin;
  char       *value_json;  // malloc'd, may be NULL
  bool        has_xp_gain;
  int64_t     xp_gain;
} hs_event_t;

int     db_open(const char *path);
void    db_close(void);
int     db_login_psn(const char *account_id, const char *online_id, const char *issuer_id,
                     const char *region, const char *locale, json_t *seed,
                     char user_id[HS_ID_LEN + 1], bool *created);
int     db_issue_tokens(const char *user_id, char access[HS_TOKEN_LEN + 1], char refresh[HS_TOKEN_LEN + 1]);
// 0 ok, 1 unknown or expired refresh token, -1 database error
int     db_refresh_tokens(const char *refresh_in, char user_id[HS_ID_LEN + 1],
                          char access[HS_TOKEN_LEN + 1], char refresh[HS_TOKEN_LEN + 1]);
// 0 ok, 1 no such live token, -1 database error
int     db_user_for_token(const char *access, char user_id[HS_ID_LEN + 1]);
int     db_user_for_latest_token(char user_id[HS_ID_LEN + 1]);
// Full UserProfile object, Extensions filtered to names (array of strings) when non-NULL.
json_t *db_get_profile(const char *user_id, json_t *names);
int     db_update_extensions(const char *user_id, json_t *extensions);
// Stores events in order and writes each one's server ticks to ticks_out.
int     db_save_events(const char *user_id, const hs_event_t *events, size_t n, int64_t *ticks_out);

// ---------------------------------------------------------------- handlers.c
void app_handle(http_req_t *req, http_res_t *res);
int  app_init(void);

// ---------------------------------------------------------------- patcher.c
void patcher_start(void);
void patcher_status(char *out, size_t n);  // one line for /healthz
