#!/bin/sh
# Runs the full Peacock server (upstream/Peacock, branch ps5-port) on this Mac as the backend for
# the PS5 hitman-server proxy. State lives in runtime/peacock/, never in the source tree.
#
#   scripts/backend.sh build      install dependencies, pack resources, build chunk0.js + web UI
#   scripts/backend.sh ps5pkg     (re)build the PS5 dynamic resources package from the console's
#                                 own game files (needs the console on, PS5_HOST/FTP_PORT) and the
#                                 rebranded locale; writes runtime/peacock/ps5/dynamic_resources_ps5.rpkg
#   scripts/backend.sh start      start on 0.0.0.0:$PEACOCK_PORT (default 8410)
#   scripts/backend.sh stop|status|logs
set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC="$ROOT/upstream/Peacock"
RUN="${PEACOCK_RUNTIME:-$ROOT/runtime/peacock}"
PORT="${PEACOCK_PORT:-8410}"
NODE="$ROOT/.tools/bin/node"
YARN="$ROOT/.tools/bin/yarn"
PIDFILE="$RUN/peacock.pid"
LOG="$RUN/peacock.log"

running() { [ -f "$PIDFILE" ] && kill -0 "$(cat "$PIDFILE")" 2>/dev/null; }

case "${1:-}" in
  build)
    cd "$SRC"
    "$YARN" install --immutable
    "$YARN" build
    ;;
  ps5pkg)
    host="${PS5_HOST:-192.168.0.37}"
    ftp="${FTP_PORT:-2121}"
    "$NODE" --version >/dev/null 2>&1 || true
    python3 -I "$ROOT/tools/ps5_dynamic_resources.py" \
      --chunk0 "ftp://$host:$ftp/data/homebrew/PPSA16387-app0_HITMAN_3/runtime/chunk0.rpkg" \
      --peacock "$SRC/resources/dynamic_resources_h3.rpkg" \
      --locale "$SRC/resources/locale.json" \
      --out "$RUN/ps5/dynamic_resources_ps5.rpkg"
    ;;
  start)
    if running; then echo "already running (pid $(cat "$PIDFILE"))"; exit 0; fi
    [ -f "$SRC/chunk0.js" ] || { echo "no build: run $0 build first"; exit 1; }
    mkdir -p "$RUN"
    # First start: PS5 defaults. Update checks and community leaderboards both contact
    # third-party hosts; PS5 runs are never submitted (scoreHandler.ts) but keep both off.
    if [ ! -f "$RUN/options.ini" ]; then
      printf '[peacock]\nupdateChecking=false\nleaderboards=false\nps5Entitlements=ALL\n' > "$RUN/options.ini"
    fi
    cd "$RUN"
    PORT="$PORT" HOST=0.0.0.0 nohup "$NODE" "$SRC/chunk0.js" >> "$LOG" 2>&1 &
    echo $! > "$PIDFILE"
    for _ in $(seq 1 100); do
      if curl -fsS -o /dev/null "http://127.0.0.1:$PORT/files/onlineconfig.json" 2>/dev/null; then
        echo "Peacock $(git -C "$SRC" rev-parse --short HEAD) on 0.0.0.0:$PORT (pid $(cat "$PIDFILE")), data in $RUN"
        exit 0
      fi
      running || { echo "exited during startup, see $LOG"; tail -20 "$LOG"; exit 1; }
      sleep 0.2
    done
    echo "not answering after 20 s, see $LOG"; exit 1
    ;;
  stop)
    if running; then kill "$(cat "$PIDFILE")"; rm -f "$PIDFILE"; echo stopped; else echo "not running"; fi
    ;;
  status)
    if running; then echo "running (pid $(cat "$PIDFILE")) on :$PORT"; else echo "not running"; exit 1; fi
    ;;
  logs)
    tail -n "${2:-100}" "$LOG"
    ;;
  *)
    echo "usage: $0 build|ps5pkg|start|stop|status|logs [n]"; exit 2
    ;;
esac
