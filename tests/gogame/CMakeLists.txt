cmake_minimum_required(VERSION 2.8)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(gogame_tests
        gogame_basic_check.cpp
        gogame_move_check.cpp)

target_link_libraries(gogame_tests gtest gtest_main)
target_link_libraries(gogame_tests gogame)
