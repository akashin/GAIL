cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)

set(PROJECT_NAME gail)
project(${PROJECT_NAME})

################################
# Games
################################
add_executable(tic_tac_toe src/games/tic_tac_toe/tic_tac_toe.cpp)
add_executable(fantastic_four src/games/fantastic_four/fantastic_four.cpp)
add_executable(code_vs_zombies src/games/code_vs_zombies/code_vs_zombies.cpp)
add_executable(knapsack src/games/knapsack/knapsack.cpp)

################################
# Testing
################################
# Add gtest.
add_subdirectory(src/third_party/googletest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Collect all tests into a single target.
set(PROJECT_TEST_NAME ${PROJECT_NAME}_tests)
file(GLOB_RECURSE TEST_SRC_FILES "src/tests/*.cpp")
add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
target_link_libraries(${PROJECT_TEST_NAME} gtest gtest_main)
