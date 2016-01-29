cmake_minimum_required(VERSION 2.8)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(neuralnet_tests
        basic_check.cpp)

target_link_libraries(neuralnet_tests gtest gtest_main)
target_link_libraries(neuralnet_tests neuralnet)
