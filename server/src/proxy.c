// Proxy mode: the game keeps talking to http://127.0.0.1:8000 (the in-memory patch has only 15
// bytes for the domain, config.c), and every request is forwarded here to the full Peacock
// server at g_cfg.upstream. Peacock owns all game state; this file only moves bytes.
//
//  * One upstream connection per request (Connection: close), so framing errors cannot leak
//    from one exchange into the next.
//  * The request target and Host header go through unchanged, so Peacock builds every URL it
//    hands out (blob URLs, onlineconfig, menus) against 127.0.0.1:8000 and the game comes back
//    through this proxy.
//  * The response is read completely (Content-Length, chunked or until close), then relayed with
//    its status, end-to-end headers and body bytes as they were.
//  * The client only adds "Authorization: bearer <token>" when the URL scheme is https or wss
//    (sub_c43dee, docs/PS5_GHIDRA_FINDINGS.md). The patch has to switch it to http, so the game
//    stops sending its token. The token it got from /oauth/token passes through here, so it is
//    remembered and attached to the game's later requests that have no Authorization header,
//    which is exactly the header the game would have sent over https.
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "hs.h"

#define RESP_HEAD_LIMIT (64 * 1024)
#define RESP_BODY_LIMIT (64 * 1024 * 1024)

// ------------------------------------------------------------------ session token

static pthread_mutex_t g_tok_lock = PTHREAD_MUTEX_INITIALIZER;
static char            g_token[4096];

void proxy_remember_token(const char *t) {
  pthread_mutex_lock(&g_tok_lock);
  bool changed = strcmp(g_token, t ? t : "") != 0;
  snprintf(g_token, sizeof(g_token), "%s", t ? t : "");
  pthread_mutex_unlock(&g_tok_lock);
  if (changed) LOGI("proxy: the game logged in; its token (%zu bytes) is attached to its requests from now on",
                    strlen(t ? t : ""));
}

bool proxy_session_token(char *out, size_t n) {
  pthread_mutex_lock(&g_tok_lock);
  bool ok = g_token[0] && strlen(g_token) < n;
  if (ok) snprintf(out, n, "%s", g_token);
  pthread_mutex_unlock(&g_tok_lock);
  return ok;
}

// ------------------------------------------------------------------ growable buffer

typedef struct {
  char  *p;
  size_t len, cap;
  bool   oom;
} buf_t;

static void buf_add(buf_t *b, const void *data, size_t n) {
  if (b->oom) return;
  if (b->len + n + 1 > b->cap) {
    size_t cap = b->cap ? b->cap : 1024;
    while (b->len + n + 1 > cap) cap *= 2;
    char *np = realloc(b->p, cap);
    if (!np) {
      b->oom = true;
      return;
    }
    b->p = np;
    b->cap = cap;
  }
  memcpy(b->p + b->len, data, n);
  b->len += n;
  b->p[b->len] = '\0';
}

static void buf_fmt(buf_t *b, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
static void buf_fmt(buf_t *b, const char *fmt, ...) {
  char tmp[1024];
  va_list ap;
  va_start(ap, fmt);
  int n = vsnprintf(tmp, sizeof(tmp), fmt, ap);
  va_end(ap);
  if (n < 0) return;
  if ((size_t)n < sizeof(tmp)) {
    buf_add(b, tmp, (size_t)n);
    return;
  }
  char *big = malloc((size_t)n + 1);
  if (!big) {
    b->oom = true;
    return;
  }
  va_start(ap, fmt);
  vsnprintf(big, (size_t)n + 1, fmt, ap);
  va_end(ap);
  buf_add(b, big, (size_t)n);
  free(big);
}

// ------------------------------------------------------------------ upstream socket

static uint64_t g_forwarded, g_failed;
static char     g_last_error[160];

static void set_last_error(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static void set_last_error(const char *fmt, ...) {
  char tmp[sizeof(g_last_error)];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(tmp, sizeof(tmp), fmt, ap);
  va_end(ap);
  pthread_mutex_lock(&g_tok_lock);
  memcpy(g_last_error, tmp, sizeof(tmp));
  pthread_mutex_unlock(&g_tok_lock);
}

static int resolve(struct sockaddr_in *sa) {
  memset(sa, 0, sizeof(*sa));
  sa->sin_family = AF_INET;
  sa->sin_port = htons((uint16_t)g_cfg.upstream_port);
  if (inet_pton(AF_INET, g_cfg.upstream_host, &sa->sin_addr) == 1) return 0;
#ifndef __PROSPERO__
  struct addrinfo hints = {0}, *ai = NULL;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  if (getaddrinfo(g_cfg.upstream_host, NULL, &hints, &ai) == 0 && ai) {
    sa->sin_addr = ((struct sockaddr_in *)ai->ai_addr)->sin_addr;
    freeaddrinfo(ai);
    return 0;
  }
#endif
  return -1;
}

// Connected socket with send/receive timeouts, or -1 (errno set). Connect waits at most
// connect_ms so an absent Mac fails fast instead of holding the game for the full timeout.
static int connect_upstream(int connect_ms, int io_s) {
  struct sockaddr_in sa;
  if (resolve(&sa)) {
    errno = EHOSTUNREACH;
    return -1;
  }
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) return -1;
  int fl = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, fl | O_NONBLOCK);
  int rc = connect(fd, (struct sockaddr *)&sa, sizeof(sa));
  if (rc && errno == EINPROGRESS) {
    struct pollfd pfd = {fd, POLLOUT, 0};
    rc = poll(&pfd, 1, connect_ms);
    if (rc == 1) {
      int soerr = 0;
      socklen_t sl = sizeof(soerr);
      getsockopt(fd, SOL_SOCKET, SO_ERROR, &soerr, &sl);
      rc = soerr ? (errno = soerr, -1) : 0;
    } else {
      if (rc == 0) errno = ETIMEDOUT;
      rc = -1;
    }
  }
  if (rc) {
    int e = errno;
    close(fd);
    errno = e;
    return -1;
  }
  fcntl(fd, F_SETFL, fl & ~O_NONBLOCK);
  struct timeval tv = {io_s, 0};
  setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
  int one = 1;
  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
#ifdef SO_NOSIGPIPE
  setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &one, sizeof(one));
#endif
  return fd;
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

// ------------------------------------------------------------------ response reader

typedef struct {
  int    fd;
  buf_t  in;   // received, not yet consumed
  size_t pos;  // consumed bytes of in
  bool   eof;
} rd_t;

static int rd_fill(rd_t *r) {
  if (r->eof) return -1;
  char tmp[65536];
  for (;;) {
    ssize_t n = recv(r->fd, tmp, sizeof(tmp), 0);
    if (n < 0 && errno == EINTR) continue;
    if (n < 0) return -2;  // timeout or reset
    if (n == 0) {
      r->eof = true;
      return -1;
    }
    buf_add(&r->in, tmp, (size_t)n);
    return r->in.oom ? -3 : 0;
  }
}

static size_t rd_avail(rd_t *r) { return r->in.len - r->pos; }

// One CRLF line (CRLF stripped) of at most max-1 bytes. 0 ok, <0 error.
static int rd_line(rd_t *r, char *out, size_t max) {
  for (;;) {
    char *start = r->in.p ? r->in.p + r->pos : NULL;
    char *nl = start ? memchr(start, '\n', rd_avail(r)) : NULL;
    if (nl) {
      size_t n = (size_t)(nl - start);
      size_t copy = n && start[n - 1] == '\r' ? n - 1 : n;
      if (copy >= max) return -4;
      memcpy(out, start, copy);
      out[copy] = '\0';
      r->pos += n + 1;
      return 0;
    }
    if (rd_avail(r) > max + 2) return -4;
    int rc = rd_fill(r);
    if (rc) return rc;
  }
}

static int rd_exact(rd_t *r, buf_t *body, size_t n) {
  while (rd_avail(r) < n) {
    int rc = rd_fill(r);
    if (rc) return rc;
  }
  buf_add(body, r->in.p + r->pos, n);
  r->pos += n;
  return body->oom ? -3 : 0;
}

static bool hop_by_hop(const char *name) {
  static const char *const H[] = {"Connection",    "Keep-Alive", "Proxy-Connection", "Proxy-Authenticate",
                                  "Proxy-Authorization", "TE", "Trailer",          "Transfer-Encoding",
                                  "Upgrade",       "Content-Length", "Server"};
  for (size_t i = 0; i < sizeof(H) / sizeof(H[0]); i++)
    if (str_ieq(name, H[i])) return true;
  return false;
}

// Reads one complete response. Fills status, the end-to-end header lines (CRLF terminated) and
// the body. Returns 0, or a negative error with *why set.
static int read_response(rd_t *r, bool head_request, int *status, buf_t *headers, char ctype[256],
                         long long *clen_out, buf_t *body, const char **why) {
  char line[8192];
  for (;;) {  // skip interim 1xx responses
    if (rd_line(r, line, sizeof(line))) {
      *why = "no status line";
      return -1;
    }
    int major, minor, st;
    if (sscanf(line, "HTTP/%d.%d %d", &major, &minor, &st) != 3 || st < 100 || st > 999) {
      *why = "bad status line";
      return -1;
    }
    *status = st;
    headers->len = 0;
    ctype[0] = '\0';
    bool chunked = false;
    long long clen = -1;
    size_t head_bytes = 0;
    for (;;) {
      if (rd_line(r, line, sizeof(line))) {
        *why = "truncated headers";
        return -1;
      }
      if (!line[0]) break;
      head_bytes += strlen(line);
      if (head_bytes > RESP_HEAD_LIMIT) {
        *why = "headers too large";
        return -1;
      }
      char *colon = strchr(line, ':');
      if (!colon || colon == line) {
        *why = "malformed header";
        return -1;
      }
      *colon = '\0';
      char *v = colon + 1;
      while (*v == ' ' || *v == '\t') v++;
      if (str_ieq(line, "Transfer-Encoding")) {
        if (!str_ieq(v, "chunked")) {
          *why = "unsupported transfer-encoding";
          return -1;
        }
        chunked = true;
      } else if (str_ieq(line, "Content-Length")) {
        if (!*v || strspn(v, "0123456789") != strlen(v)) {
          *why = "bad content-length";
          return -1;
        }
        long long n = strtoll(v, NULL, 10);
        if (clen >= 0 && clen != n) {
          *why = "conflicting content-length";
          return -1;
        }
        clen = n;
      } else if (str_ieq(line, "Content-Type")) {
        snprintf(ctype, 256, "%s", v);
      } else if (!hop_by_hop(line)) {
        buf_fmt(headers, "%s: %s\r\n", line, v);
      }
    }
    if (st >= 100 && st < 200 && st != 101) continue;
    *clen_out = clen;
    if (head_request || st == 204 || st == 304) return 0;
    if (chunked && clen >= 0) {
      *why = "transfer-encoding with content-length";
      return -1;
    }
    if (clen > RESP_BODY_LIMIT) {
      *why = "body too large";
      return -1;
    }
    if (clen >= 0) {
      if (rd_exact(r, body, (size_t)clen)) {
        *why = "truncated body";
        return -1;
      }
      return 0;
    }
    if (chunked) {
      for (;;) {
        if (rd_line(r, line, sizeof(line))) {
          *why = "truncated chunk";
          return -1;
        }
        size_t digits = strspn(line, "0123456789abcdefABCDEF");
        if (!digits || digits > 8 || (line[digits] && line[digits] != ';')) {
          *why = "bad chunk size";
          return -1;
        }
        size_t sz = (size_t)strtoull(line, NULL, 16);
        if (sz == 0) {
          do {
            if (rd_line(r, line, sizeof(line))) {
              *why = "truncated trailers";
              return -1;
            }
          } while (line[0]);
          return 0;
        }
        if (sz > RESP_BODY_LIMIT - body->len) {
          *why = "body too large";
          return -1;
        }
        if (rd_exact(r, body, sz) || rd_line(r, line, sizeof(line)) || line[0]) {
          *why = "truncated chunk";
          return -1;
        }
      }
    }
    // No framing: the body runs until the upstream closes.
    for (;;) {
      if (rd_avail(r)) {
        buf_add(body, r->in.p + r->pos, rd_avail(r));
        r->pos = r->in.len;
      }
      if (body->len > RESP_BODY_LIMIT) {
        *why = "body too large";
        return -1;
      }
      int rc = rd_fill(r);
      if (rc == -1) return 0;
      if (rc) {
        *why = "read failed";
        return -1;
      }
    }
  }
}

// ------------------------------------------------------------------ forwarding

static void preview(const char *p, size_t n, char *out, size_t outlen) {
  size_t o = 0;
  for (size_t i = 0; i < n && o + 4 < outlen; i++) {
    unsigned char ch = (unsigned char)p[i];
    out[o++] = (ch >= 32 && ch < 127) ? (char)ch : '.';
  }
  if (n > 0 && o + 4 >= outlen) {
    out[o++] = '.';
    out[o++] = '.';
  }
  out[o] = '\0';
}

static void fail(http_res_t *res, int status, const char *error, const char *detail, const http_req_t *req) {
  __atomic_add_fetch(&g_failed, 1, __ATOMIC_RELAXED);
  set_last_error("%s: %s (%s %s)", error, detail, req->method, req->path);
  LOGW("proxy: %s %s -> %d %s: %s (upstream %s)", req->is_head ? "HEAD" : req->method, req->path, status, error,
       detail, g_cfg.upstream);
  http_res_error(res, status, error, detail);
}

void proxy_forward(http_req_t *req, http_res_t *res) {
  const char *via = http_header(req, "Via");
  if (via && strstr(via, HS_NAME)) {
    fail(res, 508, "proxy_loop", "request already passed through this server", req);
    return;
  }

  // The game's own token, when it came without one (see the top of this file).
  char token[4096];
  bool attach = req->peer_loopback && !http_header(req, "Authorization") && proxy_session_token(token, sizeof(token));

  buf_t out = {0};
  const char *method = req->is_head ? "HEAD" : req->method;
  buf_fmt(&out, "%s %s HTTP/1.1\r\n", method, req->target);
  const char *host = http_header(req, "Host");
  buf_fmt(&out, "Host: %s\r\n", host && *host ? host : "127.0.0.1");
  for (int i = 0; i < req->nheaders; i++) {
    const char *n = req->headers[i].name;
    if (str_ieq(n, "Host") || str_ieq(n, "Expect") || str_ieq(n, "Via") || str_ieq(n, "X-Forwarded-For") ||
        str_ieq(n, "X-HS-Peer") || hop_by_hop(n))
      continue;
    buf_fmt(&out, "%s: %s\r\n", n, req->headers[i].value);
  }
  if (attach) buf_fmt(&out, "Authorization: bearer %s\r\n", token);
  buf_fmt(&out, "Via: 1.1 %s\r\nX-Forwarded-For: %s\r\nX-Forwarded-Proto: http\r\nX-HS-Peer: %s\r\n", HS_NAME,
          req->peer, req->peer_loopback ? "loopback" : "lan");
  bool has_body = req->body_len > 0 || !strcmp(method, "POST") || !strcmp(method, "PUT") || !strcmp(method, "PATCH");
  if (has_body) buf_fmt(&out, "Content-Length: %zu\r\n", req->body_len);
  buf_fmt(&out, "Connection: close\r\n\r\n");
  if (req->body_len) buf_add(&out, req->body, req->body_len);
  if (out.oom) {
    free(out.p);
    fail(res, 500, "proxy_error", "out of memory", req);
    return;
  }

  if (g_cfg.log_level <= LOG_DEBUG) {
    char names[512] = "";
    size_t o = 0;
    for (int i = 0; i < req->nheaders && o + 2 < sizeof(names); i++)
      o += (size_t)snprintf(names + o, sizeof(names) - o, "%s%s", i ? "," : "", req->headers[i].name);
    char pv[400];
    if (str_ieq(req->path, "/oauth/token")) http_redact_form(req->body ? req->body : "", pv, sizeof(pv));
    else preview(req->body ? req->body : "", req->body_len, pv, sizeof(pv));
    LOGD("proxy: > %s %s headers [%s]%s body %zu: %s", method, req->path, names,
         attach ? " +game token" : "", req->body_len, pv);
  }

  struct timeval t0, t1;
  gettimeofday(&t0, NULL);
  int fd = connect_upstream(3000, g_cfg.upstream_timeout);
  if (fd < 0) {
    char detail[160];
    snprintf(detail, sizeof(detail), "cannot connect to %s: %s", g_cfg.upstream, strerror(errno));
    free(out.p);
    fail(res, 502, "upstream_unreachable", detail, req);
    return;
  }
  int sent = send_all(fd, out.p, out.len);
  free(out.p);
  if (sent) {
    close(fd);
    fail(res, 502, "upstream_send_failed", strerror(errno), req);
    return;
  }

  rd_t r = {fd, {0}, 0, false};
  buf_t headers = {0}, body = {0};
  int status = 0;
  long long clen = -1;
  char ctype[256];
  const char *why = NULL;
  int rc = read_response(&r, req->is_head, &status, &headers, ctype, &clen, &body, &why);
  int read_errno = errno;
  close(fd);
  free(r.in.p);
  gettimeofday(&t1, NULL);
  long ms = (t1.tv_sec - t0.tv_sec) * 1000 + (t1.tv_usec - t0.tv_usec) / 1000;
  if (rc || headers.oom || body.oom) {
    free(headers.p);
    free(body.p);
    bool timeout = read_errno == EAGAIN || read_errno == EWOULDBLOCK || read_errno == ETIMEDOUT;
    fail(res, timeout ? 504 : 502, timeout ? "upstream_timeout" : "upstream_bad_response",
         why ? why : "out of memory", req);
    return;
  }
  __atomic_add_fetch(&g_forwarded, 1, __ATOMIC_RELAXED);

  // The block holds the header lines, then the Content-Type value after their NUL.
  size_t hl = headers.len, cl = strlen(ctype);
  char *block = malloc(hl + cl + 2);
  if (!block) {
    free(headers.p);
    free(body.p);
    fail(res, 500, "proxy_error", "out of memory", req);
    return;
  }
  if (hl) memcpy(block, headers.p, hl);
  block[hl] = '\0';
  memcpy(block + hl + 1, ctype, cl + 1);
  free(headers.p);

  free(res->body);
  free(res->more_headers);
  res->status = status;
  res->more_headers = block;
  res->content_type = cl ? block + hl + 1 : "application/octet-stream";
  res->body = body.p;
  res->body_len = body.len;
  res->head_length = req->is_head ? (clen >= 0 ? clen : 0) : -1;

  // The game's login response carries the token it will need on every later request.
  if (status == 200 && str_ieq(req->path, "/oauth/token") && body.len) {
    json_error_t jerr;
    json_t *j = json_loadb(body.p, body.len, 0, &jerr);
    const char *at = json_string_value(json_object_get(j, "access_token"));
    if (at && req->peer_loopback) proxy_remember_token(at);
    json_decref(j);
  }

  if (status >= 400) {
    char pv[300];
    preview(body.p ? body.p : "", body.len, pv, sizeof(pv));
    LOGW("proxy: upstream answered %s %s with %d in %ld ms: %s", method, req->path, status, ms, pv);
  } else if (g_cfg.log_level <= LOG_DEBUG) {
    char pv[400] = "";
    if (!str_ieq(req->path, "/oauth/token")) preview(body.p ? body.p : "", body.len, pv, sizeof(pv));
    LOGD("proxy: < %d %s %zu bytes in %ld ms: %s", status, ctype, body.len, ms, pv);
  }
}

void proxy_status(char *out, size_t n) {
  struct timeval t0, t1;
  gettimeofday(&t0, NULL);
  int fd = connect_upstream(1500, 2);
  gettimeofday(&t1, NULL);
  long ms = (t1.tv_sec - t0.tv_sec) * 1000 + (t1.tv_usec - t0.tv_usec) / 1000;
  char err[sizeof(g_last_error)];
  pthread_mutex_lock(&g_tok_lock);
  memcpy(err, g_last_error, sizeof(err));
  bool tok = g_token[0] != '\0';
  pthread_mutex_unlock(&g_tok_lock);
  if (fd >= 0) close(fd);
  snprintf(out, n, "upstream %s %s (%ld ms); forwarded %llu, failed %llu; game token %s%s%s", g_cfg.upstream,
           fd >= 0 ? "reachable" : "UNREACHABLE", ms, (unsigned long long)g_forwarded, (unsigned long long)g_failed,
           tok ? "held" : "not yet", err[0] ? "; last error: " : "", err);
}
