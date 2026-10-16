#!/bin/bash
# Re-search every position of a datagen text file at a new node budget. The
# order of positions and their game results are kept, only the score changes.
#
#   ./relabel.sh <nodes> <in.txt> <name>
#
#   ./relabel.sh 2500 data/e2-n40000.txt e1-n2500      data/e1-n2500.txt, .bin, .done
#   ./relabel.sh 500000 data/ref-pos.txt ref            the reference validation set
#
# Resumable the same way as gen.sh: finished chunks stay in data/<name>.parts/,
# the same command carries on, and a .done marker skips the whole run.
#
# Overridable: ENGINE JOBS BULLET_UTILS CHUNK_NODES
#
set -euo pipefail

USAGE="usage: relabel.sh <nodes> <in.txt> <name>"
NODES=${1:?$USAGE}
IN=$(realpath "${2:?$USAGE}")
NAME=${3:?$USAGE}

cd "$(dirname "$0")"

ENGINE=${ENGINE:-builds/teacher}
JOBS=${JOBS:-$(nproc)}
BULLET_UTILS=${BULLET_UTILS:-tools/bullet-utils}

[ -x "$ENGINE" ] || { echo "no teacher at $ENGINE, see the header of gen.sh" >&2; exit 1; }
[ -f "$IN" ] || { echo "no such file: $IN" >&2; exit 1; }

if [ -f "data/$NAME.done" ]; then
    echo "data/$NAME is complete, skipping"
    exit 0
fi

CHUNK_NODES=${CHUNK_NODES:-300000000}
CHUNK=$(( CHUNK_NODES / NODES ))
[ "$CHUNK" -ge 1000 ] || CHUNK=1000

PARTS="data/$NAME.parts"
mkdir -p "$PARTS"

SETTINGS="relabel nodes=$NODES in=$IN lines=$(wc -l < "$IN") chunk=$CHUNK teacher=$(md5sum < "$ENGINE" | cut -c1-12)"
if [ -f "$PARTS/settings" ]; then
    [ "$(cat "$PARTS/settings")" = "$SETTINGS" ] ||
        { echo "$PARTS was started as: $(cat "$PARTS/settings")" >&2; exit 1; }
else
    echo "$SETTINGS" > "$PARTS/settings"
    # Whole lines, contiguous chunks: concatenating the outputs restores the order.
    split -l "$CHUNK" -d -a 5 "$IN" "$PARTS/in."
fi

CHUNKS=$(find "$PARTS" -name 'in.*' | wc -l)
DONE=$(find "$PARTS" -name 'out.*' | wc -l)
echo "$SETTINGS: $CHUNKS chunks, $DONE already done, jobs=$JOBS" | tee -a "data/$NAME.log"
START=$(date +%s)

run_chunk() {
    local k=$1

    printf 'relabel %d %s %s\nquit\n' "$NODES" "$PARTS/in.$k" "$PARTS/tmp.$k" |
        "$ENGINE" > /dev/null 2> "$PARTS/$k.log"

    [ "$(wc -l < "$PARTS/tmp.$k")" -eq "$(wc -l < "$PARTS/in.$k")" ] && mv "$PARTS/tmp.$k" "$PARTS/out.$k"
}

for chunk in "$PARTS"/in.*; do
    k=${chunk##*.}
    [ -f "$PARTS/out.$k" ] && continue
    while [ "$(jobs -rp | wc -l)" -ge "$JOBS" ]; do wait -n || true; done
    run_chunk "$k" &
done
wait || true

for chunk in "$PARTS"/in.*; do
    k=${chunk##*.}
    [ -f "$PARTS/out.$k" ] ||
        { echo "chunk $k of $NAME is missing, run the same command again to resume" >&2; exit 1; }
done

for log in "$PARTS"/*.log; do tail -n 1 "$log"; done >> "data/$NAME.log"
cat "$PARTS"/out.* > "data/$NAME.txt.tmp"

[ "$(wc -l < "$IN")" -eq "$(wc -l < "data/$NAME.txt.tmp")" ] ||
    { echo "line count differs from $IN" >&2; exit 1; }
mv "data/$NAME.txt.tmp" "data/$NAME.txt"

"$BULLET_UTILS" convert --from text --input "data/$NAME.txt" --output "data/$NAME.bin.tmp" | tee -a "data/$NAME.log"
mv "data/$NAME.bin.tmp" "data/$NAME.bin"

rm -rf "$PARTS"
echo "wall $(( $(date +%s) - START )) s" | tee -a "data/$NAME.log"
touch "data/$NAME.done"
