#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "hs.h"

#define LOG_ROTATE_BYTES (4 * 1024 * 1024)

static FILE           *g_log;
static int             g_level = LOG_INFO;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

static const char *const LEVEL_NAMES[] = {"DEBUG", "INFO ", "WARN ", "ERROR"};

void log_open(const char *path, int level) {
  g_level = level;
  struct stat st;
  if (stat(path, &st) == 0 && st.st_size > LOG_ROTATE_BYTES) {
    char old[600];
    snprintf(old, sizeof(old), "%s.1", path);
    rename(path, old);
  }
  g_log = fopen(path, "a");
  if (!g_log) fprintf(stderr, "log: cannot open %s, logging to stdout only\n", path);
}

void log_msg(int level, const char *fmt, ...) {
  if (level < g_level) return;
  char stamp[32];
  iso8601_now(stamp, sizeof(stamp));
  char msg[2048];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(msg, sizeof(msg), fmt, ap);
  va_end(ap);

  pthread_mutex_lock(&g_lock);
  printf("[%s] %s %s\n", HS_NAME, LEVEL_NAMES[level], msg);
  fflush(stdout);
  if (g_log) {
    fprintf(g_log, "%s %s %s\n", stamp, LEVEL_NAMES[level], msg);
    fflush(g_log);
  }
  pthread_mutex_unlock(&g_lock);
}

#ifdef __PROSPERO__
typedef struct {
  char unused[45];
  char message[3075];
} notify_request_t;

int sceKernelSendNotificationRequest(int device, notify_request_t *req, size_t size, int blocking);

void notify(const char *fmt, ...) {
  notify_request_t req;
  memset(&req, 0, sizeof(req));
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(req.message, sizeof(req.message), fmt, ap);
  va_end(ap);
  LOGI("notify: %s", req.message);
  sceKernelSendNotificationRequest(0, &req, sizeof(req), 0);
}
#else
void notify(const char *fmt, ...) {
  char msg[1024];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(msg, sizeof(msg), fmt, ap);
  va_end(ap);
  LOGI("notify: %s", msg);
}
#endif
