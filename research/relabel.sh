#!/bin/bash
# Re-search every position of a datagen text file at a new node budget. The
# order of positions and their game results are kept, only the score changes.
#
#   ./relabel.sh <nodes> <in.txt> <name>
#
#   ./relabel.sh 2500 data/b40k.txt e1-b2k5      data/e1-b2k5.txt and .bin
#   ./relabel.sh 1000000 data/ref-pos.txt ref     the reference validation set
#
# Overridable: ENGINE JOBS BULLET_UTILS
#
set -euo pipefail

USAGE="usage: relabel.sh <nodes> <in.txt> <name>"
NODES=${1:?$USAGE}
IN=$(realpath "${2:?$USAGE}")
NAME=${3:?$USAGE}

cd "$(dirname "$0")"

ENGINE=${ENGINE:-builds/teacher}
JOBS=${JOBS:-$(nproc)}
BULLET_UTILS=${BULLET_UTILS:-tools/target/release/bullet-utils}

[ -x "$ENGINE" ] || { echo "no teacher at $ENGINE, see the header of gen.sh" >&2; exit 1; }
[ -f "$IN" ] || { echo "no such file: $IN" >&2; exit 1; }
[ -e "data/$NAME.txt" ] && { echo "data/$NAME.txt already exists" >&2; exit 1; }

mkdir -p data
PARTS=$(mktemp -d "data/$NAME.parts.XXXX")
trap 'rm -rf "$PARTS"' EXIT

echo "relabel nodes=$NODES in=$IN jobs=$JOBS engine=$ENGINE" | tee "data/$NAME.log"
START=$(date +%s)

# Whole lines, contiguous chunks: concatenating the outputs restores the order.
split -n "l/$JOBS" -d -a 3 "$IN" "$PARTS/in."

for chunk in "$PARTS"/in.*; do
    k=${chunk##*.}
    printf 'relabel %d %s %s\nquit\n' "$NODES" "$chunk" "$PARTS/out.$k" |
        "$ENGINE" > /dev/null 2> "$PARTS/$k.log" &
done
wait

for log in "$PARTS"/*.log; do tail -n 1 "$log" | tee -a "data/$NAME.log"; done
cat "$PARTS"/out.* > "data/$NAME.txt"

[ "$(wc -l < "$IN")" -eq "$(wc -l < "data/$NAME.txt")" ] ||
    { echo "line count differs from $IN" >&2; exit 1; }

"$BULLET_UTILS" convert --from text --input "data/$NAME.txt" --output "data/$NAME.bin" | tee -a "data/$NAME.log"
echo "wall $(( $(date +%s) - START )) s" | tee -a "data/$NAME.log"
