#!/bin/bash
# Self-play training data: "datagen" in parallel chunks, shuffled with a fixed
# seed and converted to bulletformat.
#
#   ./gen.sh <nodes> <positions> <seed> <name>
#
#   ./gen.sh 5000 8000000 1 e2-n5000      data/e2-n5000.txt, .bin, .log, .done
#
# Resumable: every finished chunk is kept in data/<name>.parts/, so after a
# crash or Ctrl-C the same command carries on, and a dataset with a .done
# marker is skipped. Chunk i plays from seed*100000+i whatever JOBS is, so a
# resumed dataset is byte-identical to an uninterrupted one.
#
# Every dataset of one experiment must come from the same teacher binary:
#
#   cp ../build/release/AteNika builds/teacher
#
# Overridable: ENGINE JOBS BULLET_UTILS CHUNK_NODES
#
set -euo pipefail

USAGE="usage: gen.sh <nodes> <positions> <seed> <name>"
NODES=${1:?$USAGE}
POSITIONS=${2:?$USAGE}
SEED=${3:?$USAGE}
NAME=${4:?$USAGE}

cd "$(dirname "$0")"

ENGINE=${ENGINE:-builds/teacher}
JOBS=${JOBS:-$(nproc)}
BULLET_UTILS=${BULLET_UTILS:-tools/bullet-utils}

[ -x "$ENGINE" ] || { echo "no teacher at $ENGINE, see the header of this script" >&2; exit 1; }
[ -x "$BULLET_UTILS" ] || { echo "no bullet-utils at $BULLET_UTILS, see step 0 of RUNBOOK.md" >&2; exit 1; }

if [ -f "data/$NAME.done" ]; then
    echo "data/$NAME is complete, skipping"
    exit 0
fi

# ~3e8 nodes, a few minutes of one process: the most an interruption loses.
CHUNK_NODES=${CHUNK_NODES:-300000000}
CHUNK=$(( CHUNK_NODES / NODES ))
[ "$CHUNK" -ge 1000 ] || CHUNK=1000
CHUNKS=$(( (POSITIONS + CHUNK - 1) / CHUNK ))

PARTS="data/$NAME.parts"
mkdir -p "$PARTS"

# Resuming with other settings or another teacher would mix two datasets.
SETTINGS="datagen nodes=$NODES positions=$POSITIONS seed=$SEED chunk=$CHUNK teacher=$(md5sum < "$ENGINE" | cut -c1-12)"
if [ -f "$PARTS/settings" ]; then
    [ "$(cat "$PARTS/settings")" = "$SETTINGS" ] ||
        { echo "$PARTS was started as: $(cat "$PARTS/settings")" >&2; exit 1; }
else
    echo "$SETTINGS" > "$PARTS/settings"
fi

DONE=$(find "$PARTS" -name '*.txt' | wc -l)
echo "$SETTINGS: $CHUNKS chunks, $DONE already done, jobs=$JOBS" | tee -a "data/$NAME.log"
START=$(date +%s)

run_chunk() {
    local i=$1
    local share=$(( i == CHUNKS - 1 ? POSITIONS - i * CHUNK : CHUNK ))

    printf 'datagen %d %d %d %s\nquit\n' "$NODES" "$share" $(( SEED * 100000 + i )) "$PARTS/$i.tmp" |
        "$ENGINE" > /dev/null 2> "$PARTS/$i.log"

    # Renamed only once complete, so a killed chunk is simply played again.
    [ "$(wc -l < "$PARTS/$i.tmp")" -eq "$share" ] && mv "$PARTS/$i.tmp" "$PARTS/$i.txt"
}

for ((i = 0; i < CHUNKS; i++)); do
    [ -f "$PARTS/$i.txt" ] && continue
    while [ "$(jobs -rp | wc -l)" -ge "$JOBS" ]; do wait -n || true; done
    run_chunk "$i" &
done
wait || true

for ((i = 0; i < CHUNKS; i++)); do
    [ -f "$PARTS/$i.txt" ] ||
        { echo "chunk $i of $NAME is missing, run the same command again to resume" >&2; exit 1; }
done

for ((i = 0; i < CHUNKS; i++)); do tail -n 1 "$PARTS/$i.log"; done >> "data/$NAME.log"

for ((i = 0; i < CHUNKS; i++)); do cat "$PARTS/$i.txt"; done |
    shuf --random-source=<(openssl enc -aes-256-ctr -pass pass:"$SEED" -nosalt -pbkdf2 < /dev/zero 2> /dev/null) \
    > "data/$NAME.txt.tmp"
mv "data/$NAME.txt.tmp" "data/$NAME.txt"

"$BULLET_UTILS" convert --from text --input "data/$NAME.txt" --output "data/$NAME.bin.tmp" | tee -a "data/$NAME.log"
mv "data/$NAME.bin.tmp" "data/$NAME.bin"

rm -rf "$PARTS"
echo "wall $(( $(date +%s) - START )) s" | tee -a "data/$NAME.log"
touch "data/$NAME.done"
