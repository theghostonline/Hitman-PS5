// Minimal HTTP/1.1 server: one thread per connection, keep-alive, Content-Length and chunked
// request bodies, Expect: 100-continue. The game's curl sets READFUNCTION without a body size
// (network map 5.1), which is exactly the case where curl sends chunked bodies and Expect.
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include "hs.h"

#define HEADER_LIMIT    (64 * 1024)
#define BODY_LIMIT      (16 * 1024 * 1024)
#define MAX_CONNECTIONS 64
#define IO_TIMEOUT_S    60

typedef struct {
  int    fd;
  char  *buf;  // unread bytes [0, len)
  size_t len;
  size_t cap;
  char   peer[64];
  bool   loopback;
  http_handler_t handler;
} conn_t;

static int          g_listen_fd = -1;
static volatile int g_stopping;
static int          g_active;  // open connections, updated with __atomic builtins

// ------------------------------------------------------------------ buffered socket reads

static int conn_fill(conn_t *c, size_t max_total) {
  if (c->len == c->cap) {
    if (c->cap >= max_total) return -2;
    size_t ncap = c->cap ? c->cap * 2 : 16384;
    if (ncap > max_total) ncap = max_total;
    char *nb = realloc(c->buf, ncap);
    if (!nb) return -1;
    c->buf = nb;
    c->cap = ncap;
  }
  for (;;) {
    ssize_t n = recv(c->fd, c->buf + c->len, c->cap - c->len, 0);
    if (n < 0 && errno == EINTR) continue;
    if (n <= 0) return -1;
    c->len += (size_t)n;
    return 0;
  }
}

static void conn_consume(conn_t *c, size_t n) {
  memmove(c->buf, c->buf + n, c->len - n);
  c->len -= n;
}

static int conn_read_exact(conn_t *c, char *out, size_t n) {
  size_t take = c->len < n ? c->len : n;
  memcpy(out, c->buf, take);
  conn_consume(c, take);
  while (take < n) {
    ssize_t r = recv(c->fd, out + take, n - take, 0);
    if (r < 0 && errno == EINTR) continue;
    if (r <= 0) return -1;
    take += (size_t)r;
  }
  return 0;
}

// Reads one CRLF-terminated line (without the CRLF) of at most max-1 bytes.
static int conn_read_line(conn_t *c, char *out, size_t max) {
  for (;;) {
    char *nl = memchr(c->buf, '\n', c->len);
    if (nl) {
      size_t n = (size_t)(nl - c->buf);
      size_t copy = n;
      if (copy > 0 && c->buf[copy - 1] == '\r') copy--;
      if (copy >= max) return -1;
      memcpy(out, c->buf, copy);
      out[copy] = '\0';
      conn_consume(c, n + 1);
      return 0;
    }
    if (c->len >= max + 2) return -1;
    if (conn_fill(c, HEADER_LIMIT) < 0) return -1;
  }
}

static int send_all(int fd, const char *p, size_t n) {
  while (n > 0) {
    ssize_t w = send(fd, p, n, 0);
    if (w < 0 && errno == EINTR) continue;
    if (w <= 0) return -1;
    p += w;
    n -= (size_t)w;
  }
  return 0;
}

// ------------------------------------------------------------------ request helpers

const char *http_header(const http_req_t *req, const char *name) {
  for (int i = 0; i < req->nheaders; i++) {
    if (str_ieq(req->headers[i].name, name)) return req->headers[i].value;
  }
  return NULL;
}

static int hexval(int ch) {
  if (ch >= '0' && ch <= '9') return ch - '0';
  if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
  if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
  return -1;
}

// Percent-decodes [s, s+n) into out. plus_space turns '+' into ' ' (form encoding).
static bool url_decode(const char *s, size_t n, char *out, size_t outlen, bool plus_space) {
  size_t o = 0;
  for (size_t i = 0; i < n; i++) {
    char ch = s[i];
    if (ch == '%' && i + 2 < n && hexval(s[i + 1]) >= 0 && hexval(s[i + 2]) >= 0) {
      ch = (char)(hexval(s[i + 1]) * 16 + hexval(s[i + 2]));
      i += 2;
      if (ch == '\0') return false;
    } else if (ch == '+' && plus_space) {
      ch = ' ';
    }
    if (o + 1 >= outlen) return false;
    out[o++] = ch;
  }
  out[o] = '\0';
  return true;
}

bool http_form_get(const char *form, const char *key, char *out, size_t outlen) {
  size_t klen = strlen(key);
  const char *p = form;
  while (p && *p) {
    const char *amp = strchr(p, '&');
    size_t plen = amp ? (size_t)(amp - p) : strlen(p);
    const char *eq = memchr(p, '=', plen);
    size_t nlen = eq ? (size_t)(eq - p) : plen;
    char name[128];
    if (nlen < sizeof(name) && url_decode(p, nlen, name, sizeof(name), true) && strlen(name) == klen &&
        !memcmp(name, key, klen)) {
      if (!eq) {
        if (outlen) out[0] = '\0';
        return true;
      }
      return url_decode(eq + 1, plen - nlen - 1, out, outlen, true);
    }
    p = amp ? amp + 1 : NULL;
  }
  return false;
}

// ------------------------------------------------------------------ responses

static const char *status_text(int s) {
  switch (s) {
    case 100: return "Continue";
    case 200: return "OK";
    case 204: return "No Content";
    case 400: return "Bad Request";
    case 401: return "Unauthorized";
    case 403: return "Forbidden";
    case 404: return "Not Found";
    case 405: return "Method Not Allowed";
    case 413: return "Payload Too Large";
    case 500: return "Internal Server Error";
    case 501: return "Not Implemented";
    case 503: return "Service Unavailable";
    default: return "Unknown";
  }
}

void http_res_json(http_res_t *res, int status, json_t *body) {
  free(res->body);
  res->status = status;
  res->content_type = "application/json; charset=utf-8";
  res->body = json_dumps(body, JSON_COMPACT | JSON_ENCODE_ANY);
  res->body_len = res->body ? strlen(res->body) : 0;
  json_decref(body);
  if (!res->body) {
    res->status = 500;
    res->content_type = "application/json; charset=utf-8";
    res->body = strdup("{\"error\":\"encode failed\"}");
    res->body_len = strlen(res->body);
  }
}

void http_res_empty(http_res_t *res, int status) {
  free(res->body);
  res->body = NULL;
  res->body_len = 0;
  res->status = status;
  res->content_type = NULL;
}

void http_res_error(http_res_t *res, int status, const char *error, const char *description) {
  json_t *o = json_pack("{s:s}", "error", error);
  if (description) json_object_set_new(o, "error_description", json_string(description));
  http_res_json(res, status, o);
}

static int write_response(conn_t *c, http_res_t *res, bool keep_alive, bool head_only) {
  char head[1024];
  bool has_file = res->file_path[0] != '\0';
  size_t len = has_file ? (size_t)res->file_size : res->body_len;
  int n;
  if (res->status == 204 || res->status == 100) {
    n = snprintf(head, sizeof(head), "HTTP/1.1 %d %s\r\nServer: %s/%s\r\n%sConnection: %s\r\n\r\n",
                 res->status, status_text(res->status), HS_NAME, HS_VERSION, res->extra_headers,
                 keep_alive ? "keep-alive" : "close");
    return send_all(c->fd, head, (size_t)n);
  }
  n = snprintf(head, sizeof(head),
               "HTTP/1.1 %d %s\r\nServer: %s/%s\r\nContent-Type: %s\r\nContent-Length: %zu\r\n%sConnection: %s\r\n\r\n",
               res->status, status_text(res->status), HS_NAME, HS_VERSION,
               res->content_type ? res->content_type : "application/octet-stream", len, res->extra_headers,
               keep_alive ? "keep-alive" : "close");
  if (send_all(c->fd, head, (size_t)n)) return -1;
  if (head_only) return 0;  // HEAD: headers only, no body
  if (!has_file) return res->body_len ? send_all(c->fd, res->body, res->body_len) : 0;

  int fd = open(res->file_path, O_RDONLY);
  if (fd < 0) return -1;
  char *chunk = malloc(65536);
  int rc = chunk ? 0 : -1;
  size_t left = len;
  while (rc == 0 && left > 0) {
    ssize_t r = read(fd, chunk, left < 65536 ? left : 65536);
    if (r <= 0) { rc = -1; break; }
    rc = send_all(c->fd, chunk, (size_t)r);
    left -= (size_t)r;
  }
  free(chunk);
  close(fd);
  return rc;
}

static void send_simple(conn_t *c, int status, const char *error) {
  http_res_t res = {0};
  http_res_error(&res, status, error, NULL);
  write_response(c, &res, false, false);
  free(res.body);
}

// ------------------------------------------------------------------ request parsing

// Fills req->path from the request target. Returns false on a malformed target.
static bool parse_target(http_req_t *req, char *target) {
  if (!strncasecmp(target, "http://", 7) || !strncasecmp(target, "https://", 8)) {
    char *p = strstr(target, "://") + 3;
    char *slash = strchr(p, '/');
    target = slash ? slash : (char *)"/";
  }
  char *q = strchr(target, '?');
  if (q) {
    *q = '\0';
    req->query = q + 1;
  } else {
    req->query = "";
  }
  char decoded[sizeof(req->path)];
  if (!url_decode(target, strlen(target), decoded, sizeof(decoded), false)) return false;
  size_t o = 0;
  for (const char *s = decoded; *s; s++) {
    if (*s == '/' && o > 0 && req->path[o - 1] == '/') continue;
    req->path[o++] = *s;
  }
  if (o == 0) req->path[o++] = '/';
  if (o > 1 && req->path[o - 1] == '/') o--;
  req->path[o] = '\0';
  return req->path[0] == '/';
}

// Reads one request. Returns 1 on success, 0 on clean EOF before a request, -1 on error
// (an error response has been sent when appropriate).
static int read_request(conn_t *c, http_req_t *req, char **head_out) {
  size_t hdr_len = 0;
  for (;;) {
    if (c->len >= 4) {
      char *end = NULL;
      for (size_t i = 0; i + 3 < c->len; i++) {
        if (c->buf[i] == '\r' && c->buf[i + 1] == '\n' && c->buf[i + 2] == '\r' && c->buf[i + 3] == '\n') {
          end = c->buf + i;
          break;
        }
      }
      if (end) {
        hdr_len = (size_t)(end - c->buf) + 4;
        break;
      }
    }
    int r = conn_fill(c, HEADER_LIMIT);
    if (r == -2) { send_simple(c, 413, "headers too large"); return -1; }
    if (r < 0) return c->len == 0 ? 0 : -1;
  }

  char *head = malloc(hdr_len + 1);
  if (!head) return -1;
  memcpy(head, c->buf, hdr_len);
  head[hdr_len] = '\0';
  conn_consume(c, hdr_len);
  *head_out = head;

  // Request line.
  char *line_end = strstr(head, "\r\n");
  *line_end = '\0';
  char *sp1 = strchr(head, ' ');
  char *sp2 = sp1 ? strchr(sp1 + 1, ' ') : NULL;
  if (!sp1 || !sp2 || (size_t)(sp1 - head) >= sizeof(req->method)) { send_simple(c, 400, "bad request line"); return -1; }
  *sp1 = *sp2 = '\0';
  snprintf(req->method, sizeof(req->method), "%s", head);
  if (strncmp(sp2 + 1, "HTTP/1.", 7)) { send_simple(c, 400, "unsupported protocol"); return -1; }
  if (!parse_target(req, sp1 + 1)) { send_simple(c, 400, "bad request target"); return -1; }

  // Headers.
  char *p = line_end + 2;
  while (*p && !(p[0] == '\r' && p[1] == '\n')) {
    char *e = strstr(p, "\r\n");
    *e = '\0';
    char *colon = strchr(p, ':');
    if (colon && req->nheaders < HTTP_MAX_HEADERS) {
      *colon = '\0';
      char *v = colon + 1;
      while (*v == ' ' || *v == '\t') v++;
      char *ve = v + strlen(v);
      while (ve > v && (ve[-1] == ' ' || ve[-1] == '\t')) *--ve = '\0';
      req->headers[req->nheaders].name = p;
      req->headers[req->nheaders].value = v;
      req->nheaders++;
    }
    p = e + 2;
  }
  return 1;
}

static int read_body(conn_t *c, http_req_t *req) {
  const char *te = http_header(req, "Transfer-Encoding");
  const char *cl = http_header(req, "Content-Length");
  bool chunked = te && strcasestr(te, "chunked");
  size_t declared = 0;
  if (!chunked && cl) {
    char *end;
    unsigned long long v = strtoull(cl, &end, 10);
    if (*end || end == cl) { send_simple(c, 400, "bad content-length"); return -1; }
    if (v > BODY_LIMIT) { send_simple(c, 413, "body too large"); return -1; }
    declared = (size_t)v;
  }
  const char *expect = http_header(req, "Expect");
  if (expect && str_ieq(expect, "100-continue") && (chunked || declared > 0)) {
    static const char CONT[] = "HTTP/1.1 100 Continue\r\n\r\n";
    if (send_all(c->fd, CONT, sizeof(CONT) - 1)) return -1;
  }

  if (!chunked) {
    req->body = malloc(declared + 1);
    if (!req->body) return -1;
    if (declared && conn_read_exact(c, req->body, declared)) return -1;
    req->body[declared] = '\0';
    req->body_len = declared;
    return 0;
  }

  size_t cap = 16384, len = 0;
  req->body = malloc(cap + 1);
  if (!req->body) return -1;
  for (;;) {
    char line[128];
    if (conn_read_line(c, line, sizeof(line))) { send_simple(c, 400, "bad chunk"); return -1; }
    char *end;
    unsigned long long sz = strtoull(line, &end, 16);
    if (end == line) { send_simple(c, 400, "bad chunk size"); return -1; }
    if (sz == 0) {
      do {  // trailers end with an empty line
        if (conn_read_line(c, line, sizeof(line))) return -1;
      } while (line[0]);
      break;
    }
    if (len + sz > BODY_LIMIT) { send_simple(c, 413, "body too large"); return -1; }
    if (len + sz > cap) {
      while (len + sz > cap) cap *= 2;
      char *nb = realloc(req->body, cap + 1);
      if (!nb) return -1;
      req->body = nb;
    }
    if (conn_read_exact(c, req->body + len, (size_t)sz)) return -1;
    len += (size_t)sz;
    if (conn_read_line(c, line, sizeof(line)) || line[0]) { send_simple(c, 400, "bad chunk terminator"); return -1; }
  }
  req->body[len] = '\0';
  req->body_len = len;
  return 0;
}

// ------------------------------------------------------------------ connection loop

// Printable preview of raw bytes for the log ("\x16\x03\x01..." is a TLS ClientHello).
static void preview(const char *p, size_t n, char *out, size_t outlen) {
  size_t o = 0;
  for (size_t i = 0; i < n && i < 48 && o + 5 < outlen; i++) {
    unsigned char ch = (unsigned char)p[i];
    if (ch >= 0x20 && ch < 0x7f && ch != '\\') out[o++] = (char)ch;
    else o += (size_t)snprintf(out + o, outlen - o, "\\x%02x", ch);
  }
  out[o] = '\0';
}

static void *conn_main(void *arg) {
  conn_t *c = arg;
  int served = 0;
  for (;;) {
    http_req_t req = {0};
    http_res_t res = {0};
    char *head = NULL;
    snprintf(req.peer, sizeof(req.peer), "%s", c->peer);
    req.peer_loopback = c->loopback;

    int r = read_request(c, &req, &head);
    if (r <= 0) {
      // A client that never completes a request (TLS, HTTP/2, or a bare connect) would
      // otherwise leave no trace at all.
      if (!served) {
        char pv[256];
        if (head) preview(head, strlen(head), pv, sizeof(pv));
        else preview(c->buf, c->len, pv, sizeof(pv));
        LOGW("%s closed without a complete request (%s): \"%s\"", c->peer,
             head ? "unparsable request line" : c->len ? "partial data" : "no data", pv);
      }
      free(head);
      break;
    }
    served++;

    const char *conn_hdr = http_header(&req, "Connection");
    bool keep_alive = !(conn_hdr && str_ieq(conn_hdr, "close"));

    const char *upgrade = http_header(&req, "Upgrade");
    if (upgrade && strcasestr(upgrade, "websocket")) {
      LOGW("refused WebSocket upgrade on %s from %s: the client did not take Online.WebSocketsEnabled=false "
           "from the served config (see README, WebSockets)", req.path, c->peer);
      http_res_error(&res, 501, "websockets_disabled", NULL);
      write_response(c, &res, false, false);
      free(res.body);
      free(head);
      break;
    }

    if (read_body(c, &req)) { free(req.body); free(head); break; }

    // The HITMAN client HEADs some files (e.g. the privacy policy) before GETting them. Route a
    // HEAD as a GET so the handler produces the same headers, then send no body.
    bool head_only = str_ieq(req.method, "HEAD");
    if (head_only) snprintf(req.method, sizeof(req.method), "GET");

    struct timeval t0, t1;
    gettimeofday(&t0, NULL);
    res.status = 500;
    c->handler(&req, &res);
    gettimeofday(&t1, NULL);
    long ms = (t1.tv_sec - t0.tv_sec) * 1000 + (t1.tv_usec - t0.tv_usec) / 1000;
    if (res.close) keep_alive = false;

    LOGI("%s %s%s %s%s%s -> %d (%ld ms)", c->peer, head_only ? "HEAD " : "", req.method, req.path,
         req.query[0] ? "?" : "", req.query, res.status, ms);
    int w = write_response(c, &res, keep_alive, head_only);
    free(res.body);
    free(req.body);
    free(head);
    if (w || !keep_alive) break;
  }
  close(c->fd);
  free(c->buf);
  free(c);
  __atomic_sub_fetch(&g_active, 1, __ATOMIC_SEQ_CST);
  return NULL;
}

// Returns 0 when listening, otherwise an errno value.
int http_listen(const char *bind_addr, int port) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) return errno;
  int one = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  struct sockaddr_in sa = {0};
  sa.sin_family = AF_INET;
  sa.sin_port = htons((uint16_t)port);
  if (inet_pton(AF_INET, bind_addr, &sa.sin_addr) != 1) {
    close(fd);
    return EINVAL;
  }
  if (bind(fd, (struct sockaddr *)&sa, sizeof(sa)) || listen(fd, 64)) {
    int err = errno;
    close(fd);
    return err;
  }
  g_listen_fd = fd;
  return 0;
}

void http_run(http_handler_t handler) {
  int fd = g_listen_fd;
  int one = 1;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_attr_setstacksize(&attr, 512 * 1024);

  while (!g_stopping) {
    struct sockaddr_in peer;
    socklen_t plen = sizeof(peer);
    int cfd = accept(fd, (struct sockaddr *)&peer, &plen);
    if (cfd < 0) {
      if (errno == EINTR || errno == ECONNABORTED) continue;
      if (g_stopping) break;
      LOGE("accept: %s", strerror(errno));
      usleep(100000);
      continue;
    }
    struct timeval tv = {IO_TIMEOUT_S, 0};
    setsockopt(cfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(cfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    setsockopt(cfd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
#ifdef SO_NOSIGPIPE
    setsockopt(cfd, SOL_SOCKET, SO_NOSIGPIPE, &one, sizeof(one));
#endif
    conn_t *c = calloc(1, sizeof(*c));
    if (!c) { close(cfd); continue; }
    c->fd = cfd;
    c->handler = handler;
    inet_ntop(AF_INET, &peer.sin_addr, c->peer, sizeof(c->peer));
    c->loopback = (ntohl(peer.sin_addr.s_addr) >> 24) == 127;

    if (__atomic_add_fetch(&g_active, 1, __ATOMIC_SEQ_CST) > MAX_CONNECTIONS) {
      LOGW("too many connections, refusing %s", c->peer);
      send_simple(c, 503, "busy");
      close(cfd);
      free(c);
      __atomic_sub_fetch(&g_active, 1, __ATOMIC_SEQ_CST);
      continue;
    }
    pthread_t th;
    if (pthread_create(&th, &attr, conn_main, c)) {
      close(cfd);
      free(c);
      __atomic_sub_fetch(&g_active, 1, __ATOMIC_SEQ_CST);
    }
  }
  pthread_attr_destroy(&attr);
}

void http_stop(void) {
  g_stopping = 1;
  if (g_listen_fd >= 0) {
    shutdown(g_listen_fd, SHUT_RDWR);
    close(g_listen_fd);
    g_listen_fd = -1;
  }
}
