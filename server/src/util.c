#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>

#include "hs.h"

void random_hex(char *out, size_t nbytes) {
  static const char HEX[] = "0123456789abcdef";
  unsigned char buf[64];
  if (nbytes > sizeof(buf)) nbytes = sizeof(buf);
  arc4random_buf(buf, nbytes);
  for (size_t i = 0; i < nbytes; i++) {
    out[2 * i] = HEX[buf[i] >> 4];
    out[2 * i + 1] = HEX[buf[i] & 15];
  }
  out[2 * nbytes] = '\0';
}

void uuid_v4(char out[HS_ID_LEN + 1]) {
  unsigned char b[16];
  arc4random_buf(b, sizeof(b));
  b[6] = (b[6] & 0x0f) | 0x40;
  b[8] = (b[8] & 0x3f) | 0x80;
  snprintf(out, HS_ID_LEN + 1,
           "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
           b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7],
           b[8], b[9], b[10], b[11], b[12], b[13], b[14], b[15]);
}

int64_t now_seconds(void) {
  return (int64_t)time(NULL);
}

int64_t now_micros(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

void iso8601_now(char *out, size_t n) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  time_t t = tv.tv_sec;
  struct tm tm;
  gmtime_r(&t, &tm);
  char base[24];
  strftime(base, sizeof(base), "%Y-%m-%dT%H:%M:%S", &tm);
  snprintf(out, n, "%s.%03dZ", base, (int)(tv.tv_usec / 1000));
}

bool str_ieq(const char *a, const char *b) {
  return strcasecmp(a, b) == 0;
}

bool str_iprefix(const char *s, const char *prefix) {
  return strncasecmp(s, prefix, strlen(prefix)) == 0;
}

int mkdirs(const char *path) {
  char tmp[512];
  snprintf(tmp, sizeof(tmp), "%s", path);
  for (char *p = tmp + 1; *p; p++) {
    if (*p != '/') continue;
    *p = '\0';
    if (mkdir(tmp, 0777) && errno != EEXIST) return -1;
    *p = '/';
  }
  if (mkdir(tmp, 0777) && errno != EEXIST) return -1;
  return 0;
}
