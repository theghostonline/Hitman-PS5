// HITMAN 3 (PS5) local online service, payload entry point.
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "hs.h"

// Asks an instance already listening on the port to exit (loopback-only /admin/shutdown), so a
// new build can be loaded over a running one without killing processes.
static void stop_previous(int port) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) return;
  struct timeval tv = {2, 0};
  setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  struct sockaddr_in sa = {0};
  sa.sin_family = AF_INET;
  sa.sin_port = htons((uint16_t)port);
  sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  if (connect(fd, (struct sockaddr *)&sa, sizeof(sa)) == 0) {
    static const char REQ[] =
        "POST /admin/shutdown HTTP/1.1\r\nHost: 127.0.0.1\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
    char buf[256];
    if (send(fd, REQ, sizeof(REQ) - 1, 0) > 0) (void)recv(fd, buf, sizeof(buf), 0);
  }
  close(fd);
}

// <data_dir>/server.pid names the newest instance. An older one whose listener died is not asked
// to stop by the handoff (the port is free), and its patcher would keep patching the game next to
// the new one's; it exits as soon as the file names another process.
static char g_pidfile[600];

static void *generation_watch(void *arg) {
  (void)arg;
  char mine[32];
  snprintf(mine, sizeof(mine), "%d", (int)getpid());
  for (;;) {
    sleep(1);
    char cur[32] = "";
    FILE *f = fopen(g_pidfile, "r");
    if (!f) continue;
    if (!fgets(cur, sizeof(cur), f)) cur[0] = '\0';
    fclose(f);
    cur[strcspn(cur, "\r\n")] = '\0';
    if (cur[0] && strcmp(cur, mine)) {
      LOGI("instance %s took over (this is %s), exiting", cur, mine);
      _exit(0);
    }
  }
  return NULL;
}

static void claim_generation(void) {
  snprintf(g_pidfile, sizeof(g_pidfile), "%s/server.pid", g_cfg.data_dir);
  FILE *f = fopen(g_pidfile, "w");
  if (!f) {
    LOGW("cannot write %s, older instances will not notice this one", g_pidfile);
    return;
  }
  fprintf(f, "%d\n", (int)getpid());
  fclose(f);
  pthread_t th;
  if (pthread_create(&th, NULL, generation_watch, NULL) == 0) pthread_detach(th);
}

int main(int argc, char **argv) {
  signal(SIGPIPE, SIG_IGN);
  const char *config_path = argc > 1 ? argv[1] : HS_DEFAULT_CONFIG;
  int cfg_rc = config_load(config_path);
  mkdirs(g_cfg.data_dir);
  log_open(g_cfg.log_path, g_cfg.log_level);
  LOGI("%s %s starting, config %s, data %s", HS_NAME, HS_VERSION, config_path, g_cfg.data_dir);
  if (cfg_rc) {
    LOGE("invalid configuration in %s", config_path);
    notify("HITMAN local server: bad config.ini, see server.log");
    return 1;
  }
  if (db_open(g_cfg.db_path) || app_init()) {
    notify("HITMAN local server: database error, see server.log");
    return 1;
  }

  for (int attempt = 0;; attempt++) {
    int err = http_listen(g_cfg.bind, g_cfg.port);
    if (!err) break;
    if (err == EADDRINUSE && attempt == 0) {
      LOGI("port %d is in use, asking the running instance to stop", g_cfg.port);
      stop_previous(g_cfg.port);
    } else if (err != EADDRINUSE || attempt >= 50) {
      LOGE("cannot listen on %s:%d: %s", g_cfg.bind, g_cfg.port, strerror(err));
      notify("HITMAN local server: port %d unavailable", g_cfg.port);
      return 1;
    }
    usleep(100 * 1000);
  }
  LOGI("listening on %s:%d, game ver %s, rpc version %s", g_cfg.bind, g_cfg.port, g_cfg.game_ver, g_cfg.rpc_version);
  if (g_cfg.upstream[0]) LOGI("proxy mode: game requests go to %s", g_cfg.upstream);
  claim_generation();

  if (g_cfg.patch_enabled) patcher_start();
  notify("HITMAN 3 local server %s on port %d", HS_VERSION, g_cfg.port);
  http_run(app_handle);
  db_close();
  return 0;
}
