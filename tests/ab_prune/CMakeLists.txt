cmake_minimum_required(VERSION 2.8)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(ab_prune_tests
        ab_prune_basic_check.cpp)

target_link_libraries(ab_prune_tests gtest gtest_main)
target_link_libraries(ab_prune_tests ab_prune)
target_link_libraries(ab_prune_tests neuralnet)
target_link_libraries(ab_prune_tests gogame)
