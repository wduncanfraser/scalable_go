cmake_minimum_required(VERSION 2.8)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(gogameab_tests
        gogameab_basic_check.cpp)

target_link_libraries(gogameab_tests gtest gtest_main)
target_link_libraries(gogameab_tests neuralnet)
target_link_libraries(gogameab_tests gogame)
target_link_libraries(gogameab_tests gogamenn)
target_link_libraries(gogameab_tests gogameab)
