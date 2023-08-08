cmake_minimum_required(VERSION 3.25)
project(ChessAI)

set(CMAKE_CXX_STANDARD 20)

include_directories(
    base/src
    base/include
    moves/src
    moves/include
)

set(SOURCES
    base/src/ZobristHash.cpp
    base/src/Pieces.cpp
    base/src/PosesHistory.cpp
    base/src/Position.cpp
    base/src/Bitboard.cpp
)

set(HEADERS
    base/include/ZobristHash.hpp
        base/include/Bitboard.hpp
    base/include/Pieces.hpp
    base/include/PosesHistory.hpp
    base/include/Position.hpp

    moves/include/Move.hpp
        moves/include/KingMask.hpp moves/include/KnightMask.hpp moves/include/BishopMask.hpp moves/include/RookMask.hpp moves/include/QueenMask.hpp moves/include/Rays.hpp)

add_executable(ChessAI main.cpp ${SOURCES} ${HEADERS})
