#!/bin/bash
# Freeze the current commit as a ladder rung: build with pinned flags, record
# the bench fingerprint, copy the binary into builds/.
#
#   ./archive.sh 0.2.0
#
set -euo pipefail
cd "$(dirname "$0")/.."

NAME=${1:?usage: archive.sh <version-or-label>}

if [ -n "$(git status --porcelain)" ]; then
  echo "working tree is dirty. Commit first, or the SHA below is a lie" >&2
  exit 1
fi

SHA=$(git rev-parse --short HEAD)
OUT="matches/builds/atenika-$NAME"

cmake -S . -B build/archive \
      -DCMAKE_BUILD_TYPE=Release \
      -DATENIKA_NATIVE=OFF \
      -DATENIKA_ARCH=x86-64-v3 \
      -DATENIKA_LTO=ON > /dev/null
cmake --build build/archive -j"$(nproc)" > /dev/null

BENCH=$(printf 'bench\nquit\n' | ./build/archive/AteNika | grep 'Nodes searched' | grep -oE '[0-9]+')

mkdir -p matches/builds
cp build/archive/AteNika "$OUT"

printf '| %-16s | %-8s | %-10s | %-10s | %s |\n' \
       "$NAME" "$SHA" "$BENCH" "$(date -I)" "" >> matches/LADDER.md
echo "archived $OUT   sha $SHA   bench $BENCH"
