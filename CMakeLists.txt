cmake_minimum_required(VERSION 3.25)
project(ChessAI)

set(CMAKE_CXX_STANDARD 20)

add_executable(ChessAI main.cpp ZobristHash.cpp pieces.cpp global.cpp PosesHistory.cpp Position.cpp)
