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
        base/src/zobrist_hash.cpp
        base/src/pieces.cpp
        base/src/poses_history.cpp
        base/src/position.cpp
        base/src/bitboard.cpp
        moves/src/sliding_attacks.cpp)

set(HEADERS
        base/include/zobrist_hash.hpp
        base/include/bitboard.hpp
        base/include/pieces.hpp
        base/include/poses_history.hpp
        base/include/position.hpp

        moves/include/move.hpp
        moves/include/sliding_attacks.hpp moves/src/rays.cpp moves/include/rays.hpp moves/src/attacks.cpp moves/include/attacks.hpp moves/src/move_array.cpp moves/include/move_array.hpp)

add_executable(ChessAI main.cpp ${SOURCES} ${HEADERS})
