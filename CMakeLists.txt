cmake_minimum_required(VERSION 3.25)
project(AteNica)
set(CMAKE_CXX_STANDARD 20)

include_directories(
    chess
)

set(SOURCES
    chess/board.cpp
    chess/rays.cpp
        chess/zobrist_hash.cpp
    chess/attacks.cpp
    chess/pawn.cpp
        chess/move_list.cpp
    chess/move.cpp
        chess/uint96.cpp
        chess/uint96.hpp
)

set(HEADERS
    chess/defs.hpp
    chess/bitfunc.hpp
)

add_executable(AteNica main.cpp ${SOURCES} ${HEADERS})
