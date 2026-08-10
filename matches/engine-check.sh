#!/bin/bash
# Confirm every binary in engines/ actually speaks UCI, and show the "id name"
# each one reports.
#
#   ./engine-check.sh
#
set -uo pipefail
cd "$(dirname "$0")"

status=0
for path in engines/*; do
  [ -f "$path" ] && [ -x "$path" ] || continue
  printf '%-20s ' "$(basename "$path")"

  # timeout: an engine that ignores "quit" would otherwise hang the loop.
  name=$(printf 'uci\nquit\n' | timeout 10 "$path" 2>/dev/null | grep -m1 '^id name') || true

  if [ -n "$name" ]; then
    echo "${name#id name }"
  else
    echo "NO RESPONSE"
    status=1
  fi
done

exit $status
