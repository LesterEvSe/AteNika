cmake_minimum_required(VERSION 3.25...4.4)

project(AteNika
    VERSION     0.1.0
    DESCRIPTION "A chess engine written in C++23"
    LANGUAGES   CXX
)

include(CheckCXXCompilerFlag)
include(CheckIPOSupported)

# Writes compile_commands.json into the build directory: the exact command line
# for every source file. Used by clang-tidy and clangd.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options

option(ATENIKA_BUILD_TESTS "Build the GoogleTest suite"                       OFF)
option(ATENIKA_LTO         "Link-time optimization in optimized builds"       ON)
option(ATENIKA_NATIVE      "Tune for this exact machine (never distribute)"   OFF)
option(ATENIKA_WARNINGS    "Enable the warning set"                           ON)
option(ATENIKA_WERROR      "Promote warnings to errors (dev and CI only)"     OFF)
option(ATENIKA_DEBUG_HASH  "Verify the incremental Zobrist key on every make" OFF)

set(ATENIKA_ARCH "x86-64-v3" CACHE STRING "Microarchitecture level for portable builds")

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

# Core library; compiled once, shared by the engine and the test binary

add_library(atenika_core OBJECT
    src/bitboard/rays.cpp
    src/bitboard/attacks.cpp

    src/core/uint96.cpp
    src/core/zobrist_hash.cpp
    src/core/move.cpp
    src/core/move_list.cpp
    src/core/board.cpp

    src/movegen/movegen.cpp
    src/movegen/perft.cpp

    src/eval/eval.cpp

    src/search/mvv_lva.cpp
    src/search/order_info.cpp
    src/search/ttable.cpp
    src/search/movepicker.cpp
    src/search/qmovepicker.cpp
    src/search/search.cpp
    src/search/bench.cpp

    src/uci/uci.cpp
    src/uci/extras.cpp

    src/book/pgn_parser.cpp
    src/book/trie_node.cpp
    src/book/book.cpp
)

target_include_directories(atenika_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

# UCI "id name" has to carry the version: fastchess and ordo identify an engine
# by that string, and a ladder of binaries all calling themselves "AteNika" is
# unusable. Single source of truth is project(VERSION) above.
target_compile_definitions(atenika_core PUBLIC ATENIKA_VERSION="${PROJECT_VERSION}")

target_compile_features(atenika_core PUBLIC cxx_std_23)
set_target_properties(atenika_core PROPERTIES
    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON
)

# Search runs on its own thread
find_package(Threads REQUIRED)
target_link_libraries(atenika_core PUBLIC Threads::Threads)

# Separate from NDEBUG on purpose: the check must be able to run on an optimized
# build, because it needs a deep perft walk to reach the rare cases.
if (ATENIKA_DEBUG_HASH)
    target_compile_definitions(atenika_core PUBLIC ATENIKA_DEBUG_HASH)
endif()


# Architecture flags

function(atenika_try_flag target flag)
    string(MAKE_C_IDENTIFIER "HAVE${flag}" probe)
    check_cxx_compiler_flag("${flag}" ${probe})
    if (${probe})
        target_compile_options(${target} PUBLIC "${flag}")
    endif()
endfunction()

if (MSVC)
    target_compile_options(atenika_core PUBLIC /arch:AVX2)
else()
    if (ATENIKA_NATIVE)
        atenika_try_flag(atenika_core "-march=native")
        atenika_try_flag(atenika_core "-mtune=native")
    elseif (ATENIKA_ARCH)
        atenika_try_flag(atenika_core "-march=${ATENIKA_ARCH}")
    endif()
endif()


# Link-time optimization

function(atenika_enable_lto target)
    if (ATENIKA_LTO AND ATENIKA_IPO_OK)
        set_target_properties(${target} PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION_RELEASE        ON
            INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON
        )
    endif()
endfunction()

# This is the single largest speedup available in the build: the engine is ~19
# translation units with small accessors (get_pieces, make, update_bitboards)
# living in .cpp files, so cross-TU inlining is otherwise blocked. Measured on
# the perft suite: -O3 = 20.5 Mnps, +march=native = 21.2, +LTO = 36.2 Mnps.
if (ATENIKA_LTO)
    check_ipo_supported(RESULT ATENIKA_IPO_OK OUTPUT ATENIKA_IPO_MSG)
    if (NOT ATENIKA_IPO_OK)
        message(WARNING "LTO requested but unsupported: ${ATENIKA_IPO_MSG}")
    endif()
endif()

atenika_enable_lto(atenika_core)


# Warnings

# The tree is warning-free. Defaults OFF so a newer compiler with new warnings cannot break
# someone else's build of a release tag.
if (ATENIKA_WARNINGS)
    if (MSVC)
        # to prevent MSVC compiler error with strerror
        target_compile_definitions(atenika_core PRIVATE _CRT_SECURE_NO_WARNINGS)
        target_compile_options(atenika_core PRIVATE /W4 /permissive- /Zc:preprocessor)
        if (ATENIKA_WERROR)
            target_compile_options(atenika_core PRIVATE /WX)
        endif()
    else()
        target_compile_options(atenika_core PRIVATE
            -Wall -Wextra
            -Wno-unused-parameter
            -Wshadow
            -Wnon-virtual-dtor
        )
        if (ATENIKA_WERROR)
            target_compile_options(atenika_core PRIVATE -Werror)
        endif()
    endif()
endif()


# Engine

add_executable(AteNika src/main.cpp)
target_link_libraries(AteNika PRIVATE atenika_core)

atenika_enable_lto(AteNika)

# Tests

if (ATENIKA_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.2
        FIND_PACKAGE_ARGS NAMES GTest       # prefer a system GTest, download only if absent
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(AteNikaTest
        tests/attacks_test.cpp
        tests/bitfunc_test.cpp
        tests/board_test.cpp
        tests/zob_hash_test.cpp
        tests/perft_test.cpp
        tests/mate_test.cpp
        tests/pv_test.cpp
        # tests/perft_comparison_test.cpp   # dead since count_bits became std::popcount
        # tests/eval_test.cpp   # re-enable once the evaluation is tapered (Phase 3.1)
    )
    target_include_directories(AteNikaTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    target_link_libraries(AteNikaTest PRIVATE atenika_core GTest::gtest GTest::gtest_main)

    # Must match atenika_core. Its objects are LLVM bitcode or GCC IR under LTO,
    # and a link line without -flto never loads the plugin that can read them —
    # "file format not recognized" at link time.
    atenika_enable_lto(AteNikaTest)

    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(AteNikaTest DISCOVERY_TIMEOUT 120)
endif()


# Configuration summary

if (MSVC)
    set(ATENIKA_ARCH_SUMMARY "AVX2")
elseif (ATENIKA_NATIVE)
    set(ATENIKA_ARCH_SUMMARY "native (this machine only — do not distribute)")
else()
    set(ATENIKA_ARCH_SUMMARY "${ATENIKA_ARCH}")
endif()

message(STATUS "AteNika ${PROJECT_VERSION}")
message(STATUS "  build type : ${CMAKE_BUILD_TYPE}")
message(STATUS "  standard   : C++23")
message(STATUS "  arch       : ${ATENIKA_ARCH_SUMMARY}")
# ATENIKA_LTO is only the request; check_ipo_supported has the final say, and it
# fails on some clang installations. Report what the build will actually do.
if (ATENIKA_LTO AND ATENIKA_IPO_OK)
    set(ATENIKA_LTO_SUMMARY "ON")
elseif (ATENIKA_LTO)
    set(ATENIKA_LTO_SUMMARY "OFF (requested, but unsupported by this toolchain)")
else()
    set(ATENIKA_LTO_SUMMARY "OFF")
endif()

message(STATUS "  LTO        : ${ATENIKA_LTO_SUMMARY}")
message(STATUS "  tests      : ${ATENIKA_BUILD_TESTS}")
message(STATUS "  -Werror    : ${ATENIKA_WERROR}")
