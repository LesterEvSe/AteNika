cmake_minimum_required(VERSION 3.25)
project(AteNica)
set(CMAKE_CXX_STANDARD 20)

include_directories(
    chess
)

set(SOURCES
    chess/main.cpp
    chess/board.cpp
    chess/rays.cpp
    chess/zobrist_hash.cpp
    chess/attacks.cpp
    chess/movepicker.cpp
    chess/move.cpp
    chess/uint96.cpp
        chess/movegen.cpp
        chess/movegen.hpp
)

set(HEADERS
    chess/defs.hpp
    chess/bitfunc.hpp
)

add_executable(AteNica ${SOURCES} ${HEADERS})
