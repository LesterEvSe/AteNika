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
    base/src/global.cpp

    main.cpp
)

set(HEADERS
    base/include/ZobristHash.hpp
    base/include/global.hpp
    base/include/Pieces.hpp
    base/include/PosesHistory.hpp
    base/include/Position.hpp

    moves/include/Move.hpp
)

add_executable(ChessAI ${SOURCES} ${HEADERS})
