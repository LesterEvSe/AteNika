#!/bin/bash
# One rating list with error bars out of any set of match PGNs, the anchor
# pinned at 0.
#
#   ./rate.sh <anchor-name> <pgn>...
#
#   ./rate.sh b10k results/*-vs-b10k-*.pgn
#
# Overridable: ORDO
#
set -euo pipefail

USAGE="usage: rate.sh <anchor-name> <pgn>..."
ANCHOR=${1:?$USAGE}
shift
[ "$#" -ge 1 ] || { echo "$USAGE" >&2; exit 1; }
PGNS=()
for pgn in "$@"; do PGNS+=("$(realpath "$pgn")"); done

cd "$(dirname "$0")"

ORDO=${ORDO:-../matches/tools/ordo}

mkdir -p results
STAMP=$(date +%Y%m%d-%H%M%S)
ALL="results/rate-$STAMP.pgn"
cat "${PGNS[@]}" > "$ALL"

# -W and -D fit white advantage and draw rate from the games, -s gives the
# error bars from simulations.
"$ORDO" -p "$ALL" -a 0 -A "$ANCHOR" -W -D -s 1000 -n "$(nproc)" \
        -o "results/rate-$STAMP.txt" -c "results/rate-$STAMP.csv"
cat "results/rate-$STAMP.txt"
