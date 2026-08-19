#!/bin/bash
# SPRT one build against another. This is the only thing that accepts or rejects
# a change that alters what the engine plays.
# For a change that is *supposed* to alter nothing, use bench instead
#
#   ./sprt.sh builds/atenika-0.3.0 builds/atenika-0.2.0            gainer  [0, 5]
#   ./sprt.sh builds/atenika-0.3.0 builds/atenika-0.2.0 major      big feature [0, 10]
#   ./sprt.sh builds/atenika-0.3.0 builds/atenika-0.2.0 nonreg     refactor [-3, 1]
#
# Each run saves its state to results/sprt-<stamp>.json every 20 games, so Ctrl-C
# loses nothing. Continue it later with:
#
#   ./sprt.sh --resume results/sprt-<stamp>.json
#
set -euo pipefail
cd "$(dirname "$0")"

FASTCHESS=${FASTCHESS:-./tools/fastchess}

if [ "${1:-}" = "--resume" ]; then
  STATE=${2:?usage: sprt.sh --resume <results/sprt-STAMP.json>}
  [ -f "$STATE" ] || { echo "no such state file: $STATE" >&2; exit 1; }

  echo "resuming $STATE"
  echo "both binaries must be the ones that started it, rebuilding either is silent"
  echo
  "$FASTCHESS" -config file="$STATE" | tee -a "${STATE%.json}.log"
  exit
fi

NEW=${1:?usage: sprt.sh <new-build> <old-build> [gainer|major|nonreg]}
OLD=${2:?usage: sprt.sh <new-build> <old-build> [gainer|major|nonreg]}
MODE=${3:-gainer}

case "$MODE" in
  gainer) ELO0=0;  ELO1=5 ;;
  major)  ELO0=0;  ELO1=10 ;;  # Faster than gainer if test big feature
  nonreg) ELO0=-3; ELO1=1 ;;
  *) echo "mode must be gainer, major or nonreg" >&2; exit 1 ;;
esac

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
STATE="results/sprt-$STAMP.json"

echo "mode $MODE [$ELO0, $ELO1]   new=$NEW   old=$OLD"
echo "Ctrl-C is safe, resume with: bash sprt.sh --resume $STATE"
echo

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
  -config outname="$STATE" \
  -pgnout file="$PGN" \
  | tee "results/sprt-$STAMP.log"

echo
echo "pgn:   $PGN"
echo "state: $STATE"
echo "Read the SPRT line, not the Elo line: the Elo estimate is biased by the"
echo "stopping rule and its interval is not a valid CI."
