#!/bin/bash
# Early stopping: of all checkpoints of one run, the one with the lowest loss on
# held-out positions labelled at the run's own budget. Appends its name to
# results/picked.txt, which plots.py --final reads.
#
#   ./pick.sh <name> <val.txt>
#
#   ./pick.sh e2-n5000 data/val-n5000.txt       prints e.g. e2-n5000-14
#
set -euo pipefail

USAGE="usage: pick.sh <name> <val.txt>"
NAME=${1:?$USAGE}
VAL=$(realpath "${2:?$USAGE}")

cd "$(dirname "$0")"

mkdir -p results
compgen -G "../trainer/checkpoints/$NAME-*" > /dev/null ||
    { echo "no checkpoints for $NAME" >&2; exit 1; }

python3 analysis/eval_loss.py "$VAL" ../trainer/checkpoints/"$NAME"-*/raw.bin > "results/pick-$NAME.csv"

BEST=$(tail -n +2 "results/pick-$NAME.csv" | LC_ALL=C sort -t, -k3,3g | head -n 1 | cut -d, -f1)
BEST=$(basename "$(dirname "$BEST")")

grep -vx "$BEST" results/picked.txt 2> /dev/null | grep -v "^$NAME-[0-9]*$" > results/picked.tmp || true
echo "$BEST" >> results/picked.tmp
mv results/picked.tmp results/picked.txt
echo "$BEST"
