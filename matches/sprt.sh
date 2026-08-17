#!/bin/bash
# SPRT one build against another. This is the only thing that accepts or rejects
# a change that alters what the engine plays.
#
# For a change that is *supposed* to alter nothing, use bench instead: SPRT costs
# twenty hours to weakly suggest what a two-second diff proves outright.
#
#   ./sprt.sh builds/atenika-0.3.0 builds/atenika-0.2.0            gainer
#   ./sprt.sh builds/atenika-0.3.0 builds/atenika-0.2.0 nonreg     refactor
#
set -euo pipefail
cd "$(dirname "$0")"

NEW=${1:?usage: sprt.sh <new-build> <old-build> [gainer|nonreg]}
OLD=${2:?usage: sprt.sh <new-build> <old-build> [gainer|nonreg]}
MODE=${3:-gainer}

case "$MODE" in
  # A gainer must beat "no change at all"; a refactor only has to prove it did
  # not cost more than 3 Elo, which converges far sooner.
  gainer) ELO0=0;  ELO1=10 ;;
  nonreg) ELO0=-3; ELO1=1 ;;
  *) echo "mode must be gainer or nonreg" >&2; exit 1 ;;
esac

FASTCHESS=${FASTCHESS:-./tools/fastchess}
BOOK=${BOOK:-./books/UHO_4060_v4.epd}
TC=${TC:-8+0.08}
# Physical cores, not threads: two engines sharing a core halve each other's NPS,
# which turns a strength test into a scheduling test. 80% leaves the OS and
# fastchess itself some room. Override with CONCURRENCY=N.
CORES=$(lscpu -p=Core,Socket 2>/dev/null | grep -v '^#' | sort -u | wc -l) || CORES=0
[ "$CORES" -gt 0 ] || CORES=$(nproc)
CONCURRENCY=${CONCURRENCY:-$(( CORES * 8 / 10 ))}
[ "$CONCURRENCY" -ge 1 ] || CONCURRENCY=1
MAX_ROUNDS=${MAX_ROUNDS:-30000}

mkdir -p results
STAMP=$(date +%Y%m%d-%H%M%S)
PGN="results/sprt-$STAMP.pgn"

"$FASTCHESS" \
  -engine cmd="$NEW" name=new \
  -engine cmd="$OLD" name=old \
  -each proto=uci tc="$TC" \
  -openings file="$BOOK" format=epd order=random \
  -sprt elo0=$ELO0 elo1=$ELO1 alpha=0.05 beta=0.05 model=normalized \
  -rounds "$MAX_ROUNDS" -games 2 -repeat \
  -concurrency "$CONCURRENCY" \
  -resign movecount=3 score=400 \
  -draw movenumber=40 movecount=8 score=10 \
  -pgnout file="$PGN" \
  | tee "results/sprt-$STAMP.log"

echo
echo "pgn: $PGN"
echo "Read the SPRT line, not the Elo line: the Elo estimate is biased by the"
echo "stopping rule and its interval is not a valid CI."
