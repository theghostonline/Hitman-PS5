#!/bin/sh
# Deploys hitman-server to the console and can roll it back.
#
#   scripts/deploy-console.sh deploy [config.ini]   back up, upload, verify, start
#   scripts/deploy-console.sh rollback <backup dir> put a backup's ELF and config back and start it
#
# Every deploy first copies the console's current ELF, config.ini, hitman.db and server.log to
# backups/<timestamp>/ (FTP), so a deploy can always be undone. Uploads are checked by reading
# them back and comparing SHA-256. Env: PS5_HOST (192.168.0.37), PM_PORT (8084), FTP_PORT (2121).
set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
HOST="${PS5_HOST:-192.168.0.37}"
PM="http://$HOST:${PM_PORT:-8084}"
FTP="ftp://$HOST:${FTP_PORT:-2121}"
ELF="$ROOT/server/hitman-server.elf"
PM_DIR=/data/pldmgr/payloads/local/hitman-server
DATA_DIR=/data/hitman-server
PMH="-H X-PLDMGR-Request:1 -H Origin:$PM"

sha() { shasum -a 256 "$1" | cut -d' ' -f1; }

remote_sha() {  # remote path -> sha256 of its contents, empty when missing
  tmp=$(mktemp)
  if curl -fsS -m 120 -o "$tmp" "$FTP$1" 2>/dev/null; then sha "$tmp"; fi
  rm -f "$tmp"
}

put() {  # local file, remote path; verified by reading it back
  curl -fsS -m 300 --ftp-create-dirs -T "$1" "$FTP$2"
  [ "$(remote_sha "$2")" = "$(sha "$1")" ] || { echo "verify failed for $2"; exit 1; }
  echo "  $2 ($(sha "$1" | cut -c1-12))"
}

backup() {
  dir="$ROOT/backups/$(date +%Y%m%d-%H%M%S)"
  mkdir -p "$dir"
  for f in "$PM_DIR/hitman-server.elf" "$DATA_DIR/config.ini" "$DATA_DIR/hitman.db" "$DATA_DIR/server.log"; do
    curl -fsS -m 120 -o "$dir/$(basename "$f")" "$FTP$f" 2>/dev/null || rm -f "$dir/$(basename "$f")"
  done
  ( cd "$dir" && shasum -a 256 ./* > SHA256SUMS 2>/dev/null || true )
  echo "$dir"
}

start() {  # loads the uploaded ELF; the new instance hands over from the running one
  curl -fsS -m 30 -X POST $PMH "$PM/loadpayload:$PM_DIR/hitman-server.elf" > /dev/null
  want="$1"
  for _ in $(seq 1 40); do
    hz=$(curl -fsS -m 3 "http://$HOST:8000/healthz" 2>/dev/null || true)
    case "$hz" in *"\"version\":\"$want\""*) echo "  healthz: $hz"; return 0 ;; esac
    sleep 0.5
  done
  echo "  /healthz does not report version $want: ${hz:-no answer}"
  return 1
}

preflight() {
  curl -fsS -m 5 -o /dev/null "$PM/" || { echo "Payload Manager not answering on $PM"; exit 1; }
  curl -fsS -m 5 --list-only "$FTP/data/" > /dev/null || { echo "FTP not answering on $FTP"; exit 1; }
}

case "${1:-}" in
  deploy)
    [ -f "$ELF" ] || { echo "build first: make -C server"; exit 1; }
    preflight
    version=$(sed -n 's/^#define HS_VERSION "\(.*\)"/\1/p' "$ROOT/server/src/hs.h")
    echo "backing up the console"
    dir=$(backup)
    echo "  $dir"
    echo "uploading"
    if [ -n "${2:-}" ]; then put "$2" "$DATA_DIR/config.ini"; fi
    # Standalone mode serves the H3 package itself; proxy mode takes it from Peacock.
    put "$ROOT/server/resources/dynamic_resources_h3.rpkg" "$DATA_DIR/resources/dynamic_resources_h3.rpkg"
    mode=$(curl -fsS $PMH "$PM/manage:check?filename=hitman-server.elf" | grep -q '"file_exists":true' && echo replace || echo new)
    curl -fsS -m 120 -X POST $PMH -H "Content-Type: application/octet-stream" --data-binary @"$ELF" \
      "$PM/manage:upload?filename=hitman-server.elf&mode=$mode" > /dev/null
    [ "$(remote_sha "$PM_DIR/hitman-server.elf")" = "$(sha "$ELF")" ] || { echo "ELF upload did not verify"; exit 1; }
    echo "  $PM_DIR/hitman-server.elf ($(sha "$ELF" | cut -c1-12), $mode)"
    echo "starting $version"
    start "$version" || { echo "roll back with: $0 rollback $dir"; exit 1; }
    ;;
  rollback)
    dir="${2:?backup directory}"
    preflight
    [ -f "$dir/hitman-server.elf" ] || { echo "no hitman-server.elf in $dir"; exit 1; }
    curl -fsS -m 120 -X POST $PMH -H "Content-Type: application/octet-stream" --data-binary @"$dir/hitman-server.elf" \
      "$PM/manage:upload?filename=hitman-server.elf&mode=replace" > /dev/null
    if [ -f "$dir/config.ini" ]; then put "$dir/config.ini" "$DATA_DIR/config.ini"
    else curl -fsS -m 30 -Q "DELE $DATA_DIR/config.ini" "$FTP/" > /dev/null 2>&1 || true; fi
    curl -fsS -m 30 -X POST $PMH "$PM/loadpayload:$PM_DIR/hitman-server.elf" > /dev/null
    sleep 3
    curl -fsS -m 3 "http://$HOST:8000/healthz"; echo
    ;;
  *)
    echo "usage: $0 deploy [config.ini] | rollback <backup dir>"; exit 2
    ;;
esac
