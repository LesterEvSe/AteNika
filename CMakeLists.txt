cmake_minimum_required(VERSION 3.25)
project(AteNica)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)
set(PROJECT_SOURCES
    src/attacks.cpp
    src/board.cpp
    src/move.cpp
    src/movegen.cpp
    src/movepicker.cpp
    src/rays.cpp
    src/uint96.cpp
    src/zobrist_hash.cpp
)

set(PROJECT_HEADERS
    src/defs.hpp
    src/bitfunc.hpp
)

add_executable(AteNica src/main.cpp ${PROJECT_SOURCES} ${PROJECT_HEADERS})


# Instruction for build the tests
execute_process(COMMAND whoami OUTPUT_VARIABLE CURRENT_USER OUTPUT_STRIP_TRAILING_WHITESPACE)
set(GTEST_ROOT /home/${CURRENT_USER}/googletest-1.14.0)

include_directories(${GTEST_ROOT}/googletest/include test)
set(TEST_SOURCES
    test/attacks_test.cpp
)
add_executable(AteNicaTest ${TEST_SOURCES} ${PROJECT_SOURCES})

enable_testing()

target_link_libraries(AteNicaTest
    ${GTEST_ROOT}/build/lib/libgtest.a
    ${GTEST_ROOT}/build/lib/libgtest_main.a
)
