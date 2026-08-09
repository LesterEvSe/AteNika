#!/bin/bash
# Play one AteNika build against reference engines, then place it on the CCRL
# scale with ordo.
#
# Answer to question: "what is my rating?". Play fixed number of games.
#
#   ./gauntlet.sh builds/atenika-0.2.0                          every engine in engines/
#   ./gauntlet.sh builds/atenika-0.2.0 stash-bot-v12 stash-bot-v13   only these
#
# The selected roster is printed before the first game, so a stray file in
# engines/ cannot quietly join the tournament.
#
set -euo pipefail
cd "$(dirname "$0")"

ENGINE=${1:?usage: gauntlet.sh <build> [opponent ...]}
shift

FASTCHESS=${FASTCHESS:-./tools/fastchess}
ORDO=${ORDO:-./tools/ordo}
BOOK=${BOOK:-./books/UHO_4060_v3.epd}
# Slower than the SPRT loop on purpose. CCRL Blitz is 2min+1s, and quoting a
# number from a much faster control adds scale error on top of match error.
TC=${TC:-10+0.1}
CONCURRENCY=${CONCURRENCY:-6}
GAMES=${GAMES:-500}

# Anchor: a version whose CCRL Blitz rating is published, not estimated. Every
# other rating is positioned relative to it, so it must be among the opponents.
ANCHOR=${ANCHOR:-stash-bot-v13}
ANCHOR_ELO=${ANCHOR_ELO:-1972}

OPPONENTS=()
if [ $# -gt 0 ]; then
  for name in "$@"; do OPPONENTS+=("engines/$name"); done
else
  # -f as well as -x: a directory is "executable" too, so a source checkout left
  # in engines/ would otherwise be entered as a player.
  for path in engines/*; do [ -f "$path" ] && [ -x "$path" ] && OPPONENTS+=("$path"); done
fi

[ ${#OPPONENTS[@]} -gt 0 ] || { echo "no opponents in engines/" >&2; exit 1; }
for path in "${OPPONENTS[@]}"; do
  [ -x "$path" ] || { echo "not executable: $path" >&2; exit 1; }
done

echo "under test : $ENGINE"
echo "opponents  : ${OPPONENTS[*]}"
echo "anchor     : $ANCHOR = $ANCHOR_ELO"
echo "$GAMES rounds each, tc=$TC, concurrency=$CONCURRENCY"
echo

ARGS=(-engine cmd="$ENGINE" name="$(basename "$ENGINE")")
for path in "${OPPONENTS[@]}"; do
  ARGS+=(-engine cmd="$path" name="$(basename "$path")")
done

mkdir -p results
STAMP=$(date +%Y%m%d-%H%M%S)
PGN="results/gauntlet-$STAMP.pgn"

# "gauntlet" = the first engine plays every other; the others never meet.
"$FASTCHESS" \
  "${ARGS[@]}" \
  -each proto=uci tc="$TC" \
  -openings file="$BOOK" format=epd order=random \
  -tournament gauntlet \
  -rounds "$GAMES" -games 2 -repeat \
  -concurrency "$CONCURRENCY" \
  -resign movecount=3 score=400 \
  -draw movenumber=40 movecount=8 score=10 \
  -pgnout file="$PGN" \
  | tee "results/gauntlet-$STAMP.log"

echo
# -W and -D fit white advantage and draw rate from the games rather than
# assuming defaults; -s runs simulations for the error bars, -n parallelises them.
"$ORDO" -p "$PGN" -a "$ANCHOR_ELO" -A "$ANCHOR" \
        -W -D -s 1000 -n "$CONCURRENCY" -o "results/ordo-$STAMP.txt"
cat "results/ordo-$STAMP.txt"

echo
echo "Match error shrinks as 1/sqrt(games), but the anchor's own CCRL error"
echo "(+-10..20) and the TC mismatch do not. Past ~2000 games you are paying"
echo "hours to shrink the smallest term."
