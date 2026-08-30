#!/bin/bash
# Build and run the AteNika tests.
#
#   bash test.sh                          everything
#   bash test.sh 'Board*'                 one fixture
#   bash test.sh 'BoardTest.white_pawns_position'    one test
#   bash test.sh '*white_mate*'           anything matching
#   bash test.sh '-Perft*'                everything except perft
#   bash test.sh -l                       list what exists
#   bash test.sh -d 'Board*'              Debug build, for gdb (~5x slower)
#   bash test.sh disabled                 the DISABLED_ ones, to see what now passes
#   bash test.sh disabled 'Mate*'         the same, narrowed to one fixture
#
set -e
cd "$(dirname "$0")"

TYPE=Release
DIR=build/tests
LIST=
ALSO_DISABLED=
DEFAULT_FILTER='*'

[ "${1:-}" = "-d" ] && { TYPE=Debug; DIR=build/tests-debug; shift; }
[ "${1:-}" = "-l" ] && { LIST=--gtest_list_tests; shift; }
[ "${1:-}" = "disabled" ] && {
  ALSO_DISABLED=--gtest_also_run_disabled_tests
  DEFAULT_FILTER='*DISABLED_*'
  shift
}

cmake -S . -B "$DIR" -DCMAKE_BUILD_TYPE=$TYPE -DATENIKA_BUILD_TESTS=ON > /dev/null
cmake --build "$DIR" --target AteNikaTest -j"$(nproc)" > /dev/null

exec "$DIR/AteNikaTest" $LIST $ALSO_DISABLED --gtest_filter="${1:-$DEFAULT_FILTER}" --gtest_color=yes
