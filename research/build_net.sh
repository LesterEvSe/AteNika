#!/bin/bash
# Builds the engine around one trained net. One build per hidden width is
# enough: match.sh loads every other net of that width through EvalFile.
#
#   ./build_net.sh <net> <hidden> <name>
#
#   git show 80668a8:nets/atenika.nnue > data/v060.nnue     0.6.0's net, 512 wide
#   ./build_net.sh data/v060.nnue 512 h512
#
# Leaves builds/<name>, from its own build tree under build/research-<name>.
#
set -euo pipefail

USAGE="usage: build_net.sh <net> <hidden> <name>"
NET=$(realpath "${1:?$USAGE}")
HIDDEN=${2:?$USAGE}
NAME=${3:?$USAGE}

cd "$(dirname "$0")"

TREE="build/research-$NAME"

# Presets resolve against the source tree, so cmake runs from the root.
cd ..
cmake --preset release -B "$TREE" -DATENIKA_NET="$NET" -DATENIKA_HIDDEN="$HIDDEN" > /dev/null
cmake --build "$TREE" -j "$(nproc)" > /dev/null

mkdir -p research/builds
cp "$TREE/AteNika" "research/builds/$NAME"
echo "research/builds/$NAME"
