cmake_minimum_required(VERSION 2.8)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(gogamenn_tests
        gogamenn_basic_check.cpp)

target_link_libraries(gogamenn_tests gtest gtest_main)
target_link_libraries(gogamenn_tests gogame)
target_link_libraries(gogamenn_tests neuralnet)
target_link_libraries(gogamenn_tests gogamenn)
