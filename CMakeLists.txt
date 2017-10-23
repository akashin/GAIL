cmake_minimum_required(VERSION 3.1)
set (CMAKE_CXX_STANDARD 17)

project(GAIL)

add_executable(tic_tac_toe src/games/tic_tac_toe/tic_tac_toe.cpp)
add_executable(fantastic_four src/games/fantastic_four/fantastic_four.cpp)
