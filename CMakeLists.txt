cmake_minimum_required(VERSION 3.22)
project(AteNica)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For execute in release version from build directory.
# cmake .. -DCMAKE_BUILD_TYPE=Release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
elseif (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/Ox")
endif()

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
    src/move_list.cpp
    src/eval.cpp
    src/pst.cpp
    src/Ttable.cpp
    src/search.cpp
    src/history.cpp
)

set(PROJECT_HEADERS
    src/defs.hpp
    src/bitfunc.hpp
)

# TODO if we build tests, then we do not need the project executable (check it)
#option(BUILD_TESTS "Build tests" OFF)
#if (NOT BUILD_TESTS)
#    return()
#endif()
add_executable(AteNica src/main.cpp ${PROJECT_SOURCES} ${PROJECT_HEADERS})


# Instruction for build the tests
# For build tests execute the command: cmake .. -DBUILD_TESTS=ON

# TODO uncomment after finish the project
#option(BUILD_TESTS "Build tests" OFF)
#if (NOT BUILD_TESTS)
#    return()
#endif()

execute_process(COMMAND whoami OUTPUT_VARIABLE CURRENT_USER OUTPUT_STRIP_TRAILING_WHITESPACE)

# You just need to change the path to the google-test folder to run everything else
set(GTEST_ROOT /home/${CURRENT_USER}/googletest)

include_directories(${GTEST_ROOT}/googletest/include test)
set(TEST_SOURCES
    test/attacks_test.cpp
    test/board_test.cpp
    test/bitfunc_test.cpp
    test/perft.cpp
    test/eval_test.cpp
    test/zob_hash_test.cpp
)
add_executable(AteNicaTest ${TEST_SOURCES} ${PROJECT_SOURCES})

enable_testing()
target_link_libraries(AteNicaTest
    ${GTEST_ROOT}/build/lib/libgtest.a
    ${GTEST_ROOT}/build/lib/libgtest_main.a
)
