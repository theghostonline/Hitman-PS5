#!/bin/sh
# HITMAN PS5 backend (macOS / Linux). Run: sh start.sh
cd "$(dirname "$0")" || exit 1
exec ./node/bin/node start.mjs "$@"
