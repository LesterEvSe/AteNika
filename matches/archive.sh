#!/bin/bash
# Freeze the current commit as a ladder rung: build with pinned flags, record
# the bench fingerprint, copy the binary into builds/.
#
#   ./archive.sh 0.2.0
#
set -euo pipefail
cd "$(dirname "$0")/.."

NAME=${1:?usage: archive.sh <version-or-label>}

# Only these can change the binary, so only these have to be committed.
# CMakeLists is in the list because it carries project(VERSION) — which becomes
# ATENIKA_VERSION in "id name" — along with the source list and the build flags.
# Docs, tests/ and matches/ are free to be dirty.
DIRTY=$(git status --porcelain -- src CMakeLists.txt)
if [ -n "$DIRTY" ]; then
  echo "uncommitted, and they affect the binary, so the SHA below would be a lie:" >&2
  echo "$DIRTY" >&2
  exit 1
fi

SHA=$(git rev-parse --short HEAD)
OUT="matches/builds/atenika-$NAME"

# The ladder is append-only, so re-running for a name already recorded would
# leave two rows claiming to be the same rung.
if grep -q "^| $NAME " matches/LADDER.md; then
  echo "$NAME is already in LADDER.md; pick another name or drop that row" >&2
  exit 1
fi

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
