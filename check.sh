#!/bin/bash
# Format and lint AteNika. The equivalent of `cargo fmt` + `cargo clippy`.
#
#   bash check.sh          reformat in place, then lint
#   bash check.sh -n       report only, change nothing (use in CI or pre-commit)
#   bash check.sh -f       also apply clang-tidy's automatic fixes
#   bash check.sh -q       quick: drop clang-analyzer-*, run on every core
#
# Flags combine: `bash check.sh -n -q`. On CI, or any machine with less memory
# than this one, cap parallelism: `JOBS=2 bash check.sh -n`.
#
# Both stages always run, so one failure does not hide the other. Exits
# non-zero if anything is unformatted or any lint fires: a silent run is clean.
#
set -uo pipefail
cd "$(dirname "$0")"

MODE=fix
QUICK=

while [ $# -gt 0 ]; do
    case "$1" in
        -n) MODE=check ;;
        -f) MODE=tidyfix ;;
        -q) QUICK=1 ;;
        *)  echo "usage: bash check.sh [-n|-f] [-q]" >&2; exit 2 ;;
    esac
    shift
done

# clang-analyzer-* can exceed 1 GB per process, so 4 rather than nproc.
if [ -n "$QUICK" ]; then
    JOBS="${JOBS:-$(nproc)}"
else
    JOBS="${JOBS:-4}"
fi

# find rather than `git ls-files`, so this works on files that are moved or
# not yet staged.
mapfile -t FILES < <(find src tests \( -name '*.cpp' -o -name '*.hpp' \) | sort)

STATUS=0

echo "==> clang-format (${#FILES[@]} files)"
if [ "$MODE" = check ]; then
    clang-format --dry-run --Werror "${FILES[@]}" || STATUS=1
else
    clang-format -i "${FILES[@]}" || STATUS=1
fi

# clang-tidy reads the compile database, so it needs a configured build. The
# tests preset is the one that covers tests/ as well as src/.
echo "==> cmake configure"
cmake --preset tests > /dev/null || STATUS=1

# .clang-tidy leaves WarningsAsErrors empty so clangd shows lints inline without
# turning them into editor errors. This script is the gate, so it promotes them.
echo "==> clang-tidy (-j $JOBS${QUICK:+, no analyzer})"
TIDY_ARGS=(-p build/tests -quiet -j "$JOBS" -warnings-as-errors='*')

# clang-tidy appends a command-line -checks to the .clang-tidy value.
[ -n "$QUICK" ] && TIDY_ARGS+=(-checks='-clang-analyzer-*')
[ "$MODE" = tidyfix ] && TIDY_ARGS+=(-fix -format)
# The ^ anchors matter: run-clang-tidy joins these with | and applies re.search,
# and FetchContent's GoogleTest lives under a path that also contains "/src/".
nice -n 10 run-clang-tidy "${TIDY_ARGS[@]}" "^$PWD/src/" "^$PWD/tests/" || STATUS=1

[ "$STATUS" -eq 0 ] && echo "==> clean"
exit "$STATUS"
