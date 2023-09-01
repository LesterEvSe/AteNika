cmake_minimum_required(VERSION 3.25)
project(ChessAI)

set(CMAKE_CXX_STANDARD 20)

include_directories(
    chess
)

set(SOURCES
        chess/board.cpp
        chess/board.hpp
)

set(HEADERS
        chess/defs.hpp
        chess/bitfunc.hpp
        chess/board.cpp
        chess/board.hpp
)

add_executable(ChessAI main.cpp ${SOURCES} ${HEADERS})
