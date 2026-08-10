#!/bin/bash
# Rate one AteNika build against a single reference engine of known rating.
# Answers "what is my rating", so: fixed games, never SPRT.
#
#   ./gauntlet.sh <build> <opponent> [games]
#
#   ./gauntlet.sh builds/atenika-0.2.0 stash-1690       2000 games
#   ./gauntlet.sh builds/atenika-0.2.0 stash-1690 100   quick check
#
# Reference engines are named <engine>-<elo>, so the rating travels with the
# binary and cannot be paired with the wrong one. Builds under builds/ have no
# suffix: their rating is what this script is here to find out.
#
# Overridable: TC CONCURRENCY BOOK FASTCHESS ORDO
#
set -euo pipefail
cd "$(dirname "$0")"

USAGE="usage: gauntlet.sh <build> <opponent-with-elo-suffix> [games]"
ENGINE=${1:?$USAGE}
OPPONENT=${2:?$USAGE}
GAMES=${3:-2000}

# Trailing number is the anchor rating ordo pins the pool to. Checked here
# rather than left to ordo, which only complains once the games are already
# played.
ELO=${OPPONENT##*-}
[[ $ELO =~ ^[0-9]+$ ]] ||
    { echo "opponent must end in -<elo>, e.g. stash-1690" >&2; exit 1; }

FASTCHESS=${FASTCHESS:-./tools/fastchess}
ORDO=${ORDO:-./tools/ordo}
BOOK=${BOOK:-./books/UHO_4060_v4.epd}
# Slower than the SPRT loop on purpose: CCRL Blitz is 2min+1s, and quoting a
# number from a much faster control adds scale error on top of match error.
TC=${TC:-10+0.1}

# Physical cores, not threads: two engines sharing a core halve each other's NPS,
# which turns a strength test into a scheduling test. 80% leaves the OS and
# fastchess itself some room. Override with CONCURRENCY=N.
CORES=$(lscpu -p=Core,Socket 2>/dev/null | grep -v '^#' | sort -u | wc -l) || CORES=0
[ "$CORES" -gt 0 ] || CORES=$(nproc)
CONCURRENCY=${CONCURRENCY:-$(( CORES * 8 / 10 ))}
[ "$CONCURRENCY" -ge 1 ] || CONCURRENCY=1

[ -f "engines/$OPPONENT" ] && [ -x "engines/$OPPONENT" ] ||
    { echo "not a runnable engine: engines/$OPPONENT" >&2; exit 1; }

# "-games 2 -repeat" makes a round a colour-reversed pair on one opening, so a
# round is two games.
ROUNDS=$(( (GAMES + 1) / 2 ))

mkdir -p results
STAMP=$(date +%Y%m%d-%H%M%S)
PGN="results/gauntlet-$STAMP.pgn"

printf '%s vs %s, anchored at %s, %d games, tc=%s, concurrency=%s\n\n' \
    "$(basename "$ENGINE")" "$OPPONENT" "$ELO" $((ROUNDS * 2)) "$TC" "$CONCURRENCY"

"$FASTCHESS" \
    -engine cmd="$ENGINE" name="$(basename "$ENGINE")" \
    -engine cmd="engines/$OPPONENT" name="$OPPONENT" \
    -each proto=uci tc="$TC" \
    -openings file="$BOOK" format=epd order=random \
    -rounds "$ROUNDS" -games 2 -repeat \
    -concurrency "$CONCURRENCY" \
    -resign movecount=3 score=400 \
    -draw movenumber=40 movecount=8 score=10 \
    -pgnout file="$PGN" | tee "results/gauntlet-$STAMP.log"

# -W and -D fit white advantage and draw rate from the games instead of assuming
# defaults; -s runs simulations for the error bars, -n parallelises them.
echo
"$ORDO" -p "$PGN" -a "$ELO" -A "$OPPONENT" \
        -W -D -s 1000 -n "$CONCURRENCY" -o "results/ordo-$STAMP.txt"
cat "results/ordo-$STAMP.txt"
