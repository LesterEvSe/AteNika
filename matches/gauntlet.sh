#!/bin/bash
# Rate one AteNika build against a single reference engine of known rating.
# Answers "what is my rating", so: fixed games, never SPRT.
#
#   ./gauntlet.sh <build> <opponent> [games]
#
#   ./gauntlet.sh builds/atenika-0.2.0 stash-1690       2000 games
#   ./gauntlet.sh builds/atenika-0.2.0 stash-1690 100   quick check
#
#   TC=10+0.1 ./gauntlet.sh <build> <opponent> [games]
#
# Reference engines are named <engine>-<elo>, so the rating travels with the
# binary and cannot be paired with the wrong one. Builds under builds/ have no
# suffix: their rating is what this script is here to find out.
#
# Each run saves its state to results/gauntlet-<stamp>.json every 20 games, so
# Ctrl-C loses nothing. Continue it later with:
#
#   ./gauntlet.sh --resume results/gauntlet-<stamp>.json
#
# Overridable: TC CONCURRENCY BOOK FASTCHESS ORDO
#
set -euo pipefail
cd "$(dirname "$0")"

FASTCHESS=${FASTCHESS:-./tools/fastchess}
ORDO=${ORDO:-./tools/ordo}

# Physical cores, not threads: two engines sharing a core halve each other's NPS,
# which turns a strength test into a scheduling test. 80% leaves the OS and
# fastchess itself some room. Override with CONCURRENCY=N.
CORES=$(lscpu -p=Core,Socket 2>/dev/null | grep -v '^#' | sort -u | wc -l) || CORES=0
[ "$CORES" -gt 0 ] || CORES=$(nproc)
CONCURRENCY=${CONCURRENCY:-$(( CORES * 8 / 10 ))}
[ "$CONCURRENCY" -ge 1 ] || CONCURRENCY=1

# -W and -D fit white advantage and draw rate from the games instead of assuming
# defaults; -s runs simulations for the error bars, -n parallelises them.
rate() {
    local stamp=$1 opponent=$2 elo=$3

    echo
    "$ORDO" -p "results/gauntlet-$stamp.pgn" -a "$elo" -A "$opponent" \
            -W -D -s 1000 -n "$CONCURRENCY" -o "results/ordo-$stamp.txt"
    cat "results/ordo-$stamp.txt"
}

if [ "${1:-}" = "--resume" ]; then
    STATE=${2:?usage: gauntlet.sh --resume <results/gauntlet-STAMP.json>}
    [ -f "$STATE" ] || { echo "no such state file: $STATE" >&2; exit 1; }

    STAMP=$(basename "$STATE" .json)
    STAMP=${STAMP#gauntlet-}
    ANCHOR="results/gauntlet-$STAMP.anchor"
    [ -f "$ANCHOR" ] || { echo "no anchor beside the state: $ANCHOR" >&2; exit 1; }

    read -r OPPONENT < "$ANCHOR"
    ELO=${OPPONENT##*-}

    echo "resuming $STATE"
    echo "both binaries must be the ones that started it, rebuilding either is silent"
    echo
    "$FASTCHESS" -config file="$STATE" | tee -a "results/gauntlet-$STAMP.log"

    rate "$STAMP" "$OPPONENT" "$ELO"
    exit
fi

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

BOOK=${BOOK:-./books/8moves_v3.pgn}
case "$BOOK" in
    *.pgn) BOOK_FORMAT=pgn ;;
    *)     BOOK_FORMAT=epd ;;
esac

# Slower than the SPRT loop on purpose: CCRL Blitz is 2min+1s, and quoting a
# number from a much faster control adds scale error on top of match error.
TC=${TC:-60+0.6}

[ -f "engines/$OPPONENT" ] && [ -x "engines/$OPPONENT" ] ||
    { echo "not a runnable engine: engines/$OPPONENT" >&2; exit 1; }

# "-games 2 -repeat" makes a round a colour-reversed pair on one opening, so a
# round is two games.
ROUNDS=$(( (GAMES + 1) / 2 ))

mkdir -p results
STAMP=$(date +%Y%m%d-%H%M%S)
PGN="results/gauntlet-$STAMP.pgn"
STATE="results/gauntlet-$STAMP.json"

echo "$OPPONENT" > "results/gauntlet-$STAMP.anchor"

printf '%s vs %s, anchored at %s, %d games, tc=%s, concurrency=%s\n' \
    "$(basename "$ENGINE")" "$OPPONENT" "$ELO" $((ROUNDS * 2)) "$TC" "$CONCURRENCY"
printf 'Ctrl-C is safe, resume with: bash gauntlet.sh --resume %s\n\n' "$STATE"

"$FASTCHESS" \
    -engine cmd="$ENGINE" name="$(basename "$ENGINE")" \
    -engine cmd="engines/$OPPONENT" name="$OPPONENT" \
    -each proto=uci tc="$TC" \
    -openings file="$BOOK" format="$BOOK_FORMAT" order=random \
    -rounds "$ROUNDS" -games 2 -repeat \
    -concurrency "$CONCURRENCY" \
    -resign movecount=3 score=400 \
    -draw movenumber=40 movecount=8 score=10 \
    -config outname="$STATE" \
    -pgnout file="$PGN" | tee "results/gauntlet-$STAMP.log"

rate "$STAMP" "$OPPONENT" "$ELO"
