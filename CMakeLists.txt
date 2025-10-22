cmake_minimum_required(VERSION 3.22)
project(AteNika)
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
    src/bitfunc.cpp
    src/mvv_lva.cpp
    src/quiescence_movepick.cpp
    src/uci.cpp
    src/pgn_parser.cpp
    src/book.cpp
    src/trie_node.cpp
    src/order_info.cpp
)

set(PROJECT_HEADERS
    src/defs.hpp
    src/bitfunc.hpp
)

# Build application by default
option(BUILD_TESTS "Build tests" OFF)
if (NOT BUILD_TESTS)
    add_executable(AteNika src/main.cpp ${PROJECT_SOURCES} ${PROJECT_HEADERS})
else()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
    )
    # For Windows builds: avoid warnings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    include_directories(test)
    set(TEST_SOURCES
        test/attacks_test.cpp
        test/board_test.cpp
        test/bitfunc_test.cpp
        test/perft.cpp
        # test/eval_test.cpp
        test/zob_hash_test.cpp
        test/perft_comparison.cpp
    )

    add_executable(AteNikaTest ${TEST_SOURCES} ${PROJECT_SOURCES})
    target_link_libraries(AteNikaTest GTest::gtest_main)

    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(AteNikaTest)
endif()
