#!/bin/bash
# Trains one research net with the settings every experiment shares, so the
# dataset is the only thing that differs between two nets.
#
#   ./train.sh <name> <data.bin>
#
#   ./train.sh e2-n5000 data/e2-n5000.bin
#
# Checkpoints land in ../trainer/checkpoints/<name>-<superbatch>/, every
# SAVE_RATE superbatches; match.sh takes their quantised.bin directly.
# 1000 batches of 16384 make a superbatch of ~16M positions, so the default
# 40 superbatches feed ~655M positions to every net, whatever its dataset size.
#
# Overridable: HIDDEN WDL SUPERBATCHES BATCHES_PER_SUPERBATCH SAVE_RATE
#
set -euo pipefail

USAGE="usage: train.sh <name> <data.bin>"
NAME=${1:?$USAGE}
DATA=$(realpath "${2:?$USAGE}")

cd "$(dirname "$0")"

export HIDDEN=${HIDDEN:-512}
export WDL=${WDL:-0.0}
export SUPERBATCHES=${SUPERBATCHES:-40}
export BATCHES_PER_SUPERBATCH=${BATCHES_PER_SUPERBATCH:-1000}
export SAVE_RATE=${SAVE_RATE:-5}
export NET_ID=$NAME

compgen -G "../trainer/checkpoints/$NAME-*" > /dev/null &&
    { echo "checkpoints for $NAME already exist" >&2; exit 1; }

mkdir -p results
(cd ../trainer && cargo run --release -q -- "$DATA") 2>&1 | tee "results/train-$NAME.log"
