cmake_minimum_required(VERSION 2.8)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(goboard_tests
        goboard_basic_check.cpp
        goboard_move_check.cpp)

target_link_libraries(goboard_tests gtest gtest_main)
target_link_libraries(goboard_tests goboard)
