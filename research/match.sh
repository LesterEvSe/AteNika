#!/bin/bash
# Fixed-games match of one net against an anchor, at fixed nodes.
# Answers "how strong is this net", so: fixed games, never SPRT.
#
#   ./match.sh <net-or-build> <anchor-net-or-build> [games]
#
#   ./match.sh ../trainer/checkpoints/e2-n5000-40/quantised.bin \
#              ../trainer/checkpoints/e2-n10000-40/quantised.bin 4000
#   NODES=50000 ./match.sh builds/teacher builds/h512 1000
#
# A net (.bin, .nnue) plays inside ENGINE through the EvalFile option and is
# named after its checkpoint directory, e2-n5000-40 above. ENGINE has to be
# built for the same hidden width: ./build_net.sh <any such net> 512 h512
#
# Every net of the research has the same architecture and speed, so a node
# limit compares evaluation quality alone and does not depend on machine load.
# The PGN goes to results/; rate.sh turns any set of them into one rating list.
#
# Overridable: ENGINE NODES TC CONCURRENCY BOOK FASTCHESS
#
set -euo pipefail

USAGE="usage: match.sh <net-or-build> <anchor-net-or-build> [games]"
NEW_TARGET=$(realpath "${1:?$USAGE}")
OLD_TARGET=$(realpath "${2:?$USAGE}")
GAMES=${3:-4000}

cd "$(dirname "$0")"

ENGINE=${ENGINE:-builds/h512}

# Fills the named array with fastchess's -engine arguments for one side.
engine_args() {
    local target=$1
    local -n out=$2

    case "$target" in
        *.bin | *.nnue)
            [ -x "$ENGINE" ] || { echo "no engine at $ENGINE for $target" >&2; exit 1; }
            local name
            if [ "$(basename "$target")" = quantised.bin ]; then
                name=$(basename "$(dirname "$target")")
            else
                name=$(basename "${target%.*}")
            fi
            out=(cmd="$ENGINE" name="$name" option.EvalFile="$(realpath "$target")")
            ;;
        *)
            [ -x "$target" ] || { echo "not a runnable engine: $target" >&2; exit 1; }
            out=(cmd="$target" name="$(basename "$target")")
            ;;
    esac
}

engine_args "$NEW_TARGET" NEW_ARGS
engine_args "$OLD_TARGET" OLD_ARGS

FASTCHESS=${FASTCHESS:-../matches/tools/fastchess}
BOOK=${BOOK:-../matches/books/UHO_4060_v4.epd}

# TC=... replaces the node limit, for the activation comparison only.
if [ -n "${TC:-}" ]; then
    LIMIT="tc=$TC"
else
    LIMIT="nodes=${NODES:-20000}"
fi

# Under a node limit a slower core only makes a game take longer, so every
# thread can play. Under a clock it would bias the result, so physical cores only.
if [ -n "${TC:-}" ]; then
    CORES=$(lscpu -p=Core,Socket 2>/dev/null | grep -v '^#' | sort -u | wc -l) || CORES=0
    [ "$CORES" -gt 0 ] || CORES=$(nproc)
    CONCURRENCY=${CONCURRENCY:-$(( CORES * 8 / 10 ))}
else
    CONCURRENCY=${CONCURRENCY:-$(nproc)}
fi
[ "$CONCURRENCY" -ge 1 ] || CONCURRENCY=1

ROUNDS=$(( (GAMES + 1) / 2 ))
NEW=${NEW_ARGS[1]#name=}
OLD=${OLD_ARGS[1]#name=}

mkdir -p results
STAMP=$(date +%Y%m%d-%H%M%S)
PGN="results/$NEW-vs-$OLD-$STAMP.pgn"

echo "$NEW vs $OLD, $((ROUNDS * 2)) games, $LIMIT, concurrency=$CONCURRENCY"

"$FASTCHESS" \
    -engine "${NEW_ARGS[@]}" \
    -engine "${OLD_ARGS[@]}" \
    -each proto=uci "$LIMIT" \
    -openings file="$BOOK" format=epd order=random \
    -rounds "$ROUNDS" -games 2 -repeat \
    -concurrency "$CONCURRENCY" \
    -resign movecount=3 score=400 \
    -draw movenumber=40 movecount=8 score=10 \
    -pgnout file="$PGN" | tee "${PGN%.pgn}.log"

echo
echo "pgn: $PGN"
