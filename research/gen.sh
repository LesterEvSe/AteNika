#!/bin/bash
# Self-play training data: JOBS processes of "datagen", shuffled with a fixed
# seed and converted to bulletformat.
#
#   ./gen.sh <nodes> <positions> <seed> <name>
#
#   ./gen.sh 5000 8000000 1 b5k      data/b5k.txt, data/b5k.bin, data/b5k.log
#
# Process k plays from seed*1000+k, so two runs with different seeds never share
# a game. Every dataset of one experiment must come from the same teacher binary:
#
#   cp ../build/release/AteNika builds/teacher
#
# Overridable: ENGINE JOBS BULLET_UTILS
#
set -euo pipefail
cd "$(dirname "$0")"

USAGE="usage: gen.sh <nodes> <positions> <seed> <name>"
NODES=${1:?$USAGE}
POSITIONS=${2:?$USAGE}
SEED=${3:?$USAGE}
NAME=${4:?$USAGE}

ENGINE=${ENGINE:-builds/teacher}
JOBS=${JOBS:-$(nproc)}
BULLET_UTILS=${BULLET_UTILS:-tools/target/release/bullet-utils}

[ -x "$ENGINE" ] || { echo "no teacher at $ENGINE, see the header of this script" >&2; exit 1; }
[ -x "$BULLET_UTILS" ] || { echo "no bullet-utils at $BULLET_UTILS, see README.md" >&2; exit 1; }
[ -e "data/$NAME.txt" ] && { echo "data/$NAME.txt already exists" >&2; exit 1; }

mkdir -p data
PARTS=$(mktemp -d "data/$NAME.parts.XXXX")
trap 'rm -rf "$PARTS"' EXIT

echo "datagen nodes=$NODES positions=$POSITIONS seed=$SEED jobs=$JOBS engine=$ENGINE" | tee "data/$NAME.log"
START=$(date +%s)

for ((k = 0; k < JOBS; k++)); do
    share=$(( POSITIONS / JOBS + (k < POSITIONS % JOBS ? 1 : 0) ))
    printf 'datagen %d %d %d %s\nquit\n' "$NODES" "$share" $(( SEED * 1000 + k )) "$PARTS/$k.txt" |
        "$ENGINE" > /dev/null 2> "$PARTS/$k.log" &
done
wait

for ((k = 0; k < JOBS; k++)); do
    tail -n 1 "$PARTS/$k.log" | tee -a "data/$NAME.log"
done

for ((k = 0; k < JOBS; k++)); do cat "$PARTS/$k.txt"; done |
    shuf --random-source=<(openssl enc -aes-256-ctr -pass pass:"$SEED" -nosalt -pbkdf2 < /dev/zero 2> /dev/null) \
    > "data/$NAME.txt"

"$BULLET_UTILS" convert --from text --input "data/$NAME.txt" --output "data/$NAME.bin" | tee -a "data/$NAME.log"
echo "wall $(( $(date +%s) - START )) s" | tee -a "data/$NAME.log"
