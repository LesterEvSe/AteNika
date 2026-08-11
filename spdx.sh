#!/bin/bash
# The SPDX licence header on every source file. What check.sh is for formatting,
# this is for the notice that says what licence the code is under.
#
#   bash spdx.sh          insert the header where it is missing
#   bash spdx.sh -n       report only, change nothing (use in CI or pre-commit)
#
# Why per-file at all, when LICENSE sits at the repo root: a root file does not
# travel with a source file copied out of the tree, and the GPL-3 text is
# byte-identical for -only and -or-later — the SPDX line is the only place this
# project actually states which of the two it is.
#
# Not named license.sh: GitHub's licence detector matches any root file whose
# name starts with "licen[sc]e", so that name put this script into the contest
# for "what licence is this repo under".
#
set -uo pipefail
cd "$(dirname "$0")"

LINE1='// SPDX-License-Identifier: GPL-3.0-or-later'
LINE2='// Copyright (C) 2023 Yevhenii Sekhin'

MODE=fix
case "${1:-}" in
    "")  ;;
    -n)  MODE=check ;;
    *)   echo "usage: bash spdx.sh [-n]" >&2; exit 2 ;;
esac

# The same file set check.sh formats, so the two cannot disagree about what
# counts as source.
mapfile -t FILES < <(find src tests \( -name '*.cpp' -o -name '*.hpp' \) | sort)

STATUS=0
ok=0
added=0
missing=0
diverged=0

for f in "${FILES[@]}"; do
    # Being the first two lines is the whole requirement, so that is the whole
    # test. A notice further down the file is not a file header.
    if [ "$(sed -n 1p "$f")" = "$LINE1" ] && [ "$(sed -n 2p "$f")" = "$LINE2" ]; then
        ok=$((ok + 1))
        continue
    fi

    # A file already carrying some notice — wrong year, wrong licence, or pushed
    # below an include — must never be prepended to: it would end up with two
    # notices that disagree, which is worse than having none.
    if grep -qE '^//[[:space:]]*(SPDX-License-Identifier|Copyright)' "$f"; then
        echo "  diverged  $f"
        diverged=$((diverged + 1))
        STATUS=1
        continue
    fi

    if [ "$MODE" = check ]; then
        echo "  missing   $f"
        missing=$((missing + 1))
        STATUS=1
        continue
    fi

    # The trailing blank line is load-bearing. clang-format treats a comment
    # sitting directly on the first #include as attached to it and will carry it
    # along while sorting includes; separated by a blank line it stays a file
    # header and is left alone.
    if { printf '%s\n%s\n\n' "$LINE1" "$LINE2"; cat "$f"; } > "$f.spdx.tmp" &&
        mv "$f.spdx.tmp" "$f"; then
        echo "  added     $f"
        added=$((added + 1))
    else
        echo "  FAILED    $f" >&2
        rm -f "$f.spdx.tmp"
        STATUS=1
    fi
done

printf '==> spdx (%d files): %d ok, %d added, %d missing, %d diverged\n' \
    "${#FILES[@]}" "$ok" "$added" "$missing" "$diverged"

[ "$STATUS" -eq 0 ] && echo "==> clean"
exit "$STATUS"
