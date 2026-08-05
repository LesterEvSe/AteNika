#!/bin/bash
# Build and run AteNika.
#
#   bash run.sh          Release
#   bash run.sh -d       Debug, for gdb
#
set -e
cd "$(dirname "$0")"

PRESET=release
[ "${1:-}" = "-d" ] && PRESET=debug

cmake --preset "$PRESET" > /dev/null
cmake --build --preset "$PRESET" -j"$(nproc)" > /dev/null

exec "./build/$PRESET/AteNika"
