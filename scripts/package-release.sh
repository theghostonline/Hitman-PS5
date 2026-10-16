#!/bin/sh
# Assembles the backend release zips (one per OS, Node bundled) and the backend source zip.
#
#   scripts/package-release.sh <version>      output in dist/
#
# Needs a current backend build (scripts/backend.sh build). The PS5 resource package is never
# included: it is built on the user's machine from their own game files on first start.
set -eu

VERSION="${1:?usage: $0 <version>}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC="$ROOT/upstream/Peacock"
NODE_VER="v$(cat "$SRC/.nvmrc" | tr -d 'v \n')"
CACHE="$ROOT/.tools/node-dist"
DIST="$ROOT/dist"

mkdir -p "$CACHE" "$DIST"
[ -f "$SRC/chunk0.js" ] || { echo "no backend build: run scripts/backend.sh build"; exit 1; }

curl -fsSL -o "$CACHE/SHASUMS256-$NODE_VER.txt" "https://nodejs.org/dist/$NODE_VER/SHASUMS256.txt"

fetch_node() {  # fetch_node <archive name>
  if [ ! -f "$CACHE/$1" ]; then curl -fsSL -o "$CACHE/$1.part" "https://nodejs.org/dist/$NODE_VER/$1"; mv "$CACHE/$1.part" "$CACHE/$1"; fi
  want=$(grep " $1\$" "$CACHE/SHASUMS256-$NODE_VER.txt" | cut -d' ' -f1)
  got=$(shasum -a 256 "$CACHE/$1" | cut -d' ' -f1)
  [ "$want" = "$got" ] || { echo "checksum mismatch for $1"; exit 1; }
}

common() {  # common <out dir>
  out="$1"
  mkdir -p "$out/resources" "$out/webui/dist"
  cp "$SRC/chunk0.js" "$SRC/LICENSE" "$SRC/THIRDPARTYNOTICES.txt" "$out/"
  cp "$SRC/resources/dynamic_resources_h1.rpkg" "$SRC/resources/dynamic_resources_h2.rpkg" \
     "$SRC/resources/dynamic_resources_h3.rpkg" "$SRC/resources/contracts.prp" \
     "$SRC/resources/locale.json" "$out/resources/"
  cp -R "$SRC/resources/challenges" "$SRC/resources/mastery" "$out/resources/"
  cp "$SRC/components/ps5/ps5_store_strings.json" "$out/resources/"
  cp "$SRC"/webui/dist/*.html "$SRC/webui/dist/THIRDPARTYNOTICES.txt" "$out/webui/dist/"
  cp -R "$SRC/webui/dist/assets" "$out/webui/dist/"
  cp "$ROOT/release/backend/start.mjs" "$ROOT/release/backend/build-ps5-package.mjs" \
     "$ROOT/release/backend/options.ini" "$ROOT/release/backend/config.ini" "$out/"
  printf '%s\n' "$VERSION" > "$out/VERSION"  # the in-game updater compares against this
  cat > "$out/SOURCE.txt" <<EOF
HITMAN PS5 backend $VERSION, AGPL-3.0 (see LICENSE).
Source code: hitman-ps5-backend-src-$VERSION.zip on the same release page,
https://github.com/theghostonline/Hitman-PS5/releases/tag/v$VERSION
EOF
}

build() {  # build <suffix> <node archive>
  name="hitman-ps5-backend-$VERSION-$1"
  out="$DIST/$name"
  rm -rf "$out" "$out.zip"
  common "$out"
  fetch_node "$2"
  tmp="$(mktemp -d)"
  case "$2" in
    *.zip)
      unzip -q "$CACHE/$2" -d "$tmp"
      mkdir -p "$out/node"
      cp "$tmp"/node-*/node.exe "$tmp"/node-*/LICENSE "$out/node/"
      cp "$ROOT/release/backend/Start Server.cmd" "$out/"
      ;;
    *)
      tar -xf "$CACHE/$2" -C "$tmp"
      mkdir -p "$out/node/bin"
      cp "$tmp"/node-*/bin/node "$out/node/bin/"
      cp "$tmp"/node-*/LICENSE "$out/node/"
      cp "$ROOT/release/backend/start.sh" "$out/"
      case "$1" in macos*) cp "$ROOT/release/backend/start.command" "$out/" ;; esac
      ;;
  esac
  rm -rf "$tmp"
  (cd "$DIST" && zip -qr -9 "$name.zip" "$name")
  echo "$DIST/$name.zip $(du -h "$DIST/$name.zip" | cut -f1)"
}

build windows-x64 "node-$NODE_VER-win-x64.zip"
build macos-arm64 "node-$NODE_VER-darwin-arm64.tar.gz"
build macos-x64 "node-$NODE_VER-darwin-x64.tar.gz"
build linux-x64 "node-$NODE_VER-linux-x64.tar.xz"

# AGPL: the source of the backend that the zips above are built from.
git -C "$SRC" archive --format=zip --prefix="hitman-ps5-backend-src-$VERSION/" -o "$DIST/hitman-ps5-backend-src-$VERSION.zip" HEAD
echo "$DIST/hitman-ps5-backend-src-$VERSION.zip"
