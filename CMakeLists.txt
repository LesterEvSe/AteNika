cmake_minimum_required(VERSION 3.25)
project(ChessAI)
set(CMAKE_CXX_STANDARD 20)

include_directories(
    chess
)

set(SOURCES
    chess/board.cpp
    chess/rays.cpp
)

set(HEADERS
    chess/defs.hpp
    chess/bitfunc.hpp
)

add_executable(AteNica main.cpp ${SOURCES} ${HEADERS})
