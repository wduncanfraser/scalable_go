cmake_minimum_required(VERSION 2.8.4)

project(Scalable_Go)

add_definitions(-std=c++11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/")

set(NET_BENCHMARK
        benchmark_neuralnet.cpp)

add_executable(benchmark_neuralnet ${NET_BENCHMARK})

include_directories(goboard neuralnet)

add_subdirectory(goboard)
add_subdirectory(neuralnet)
add_subdirectory(tests)

target_link_libraries(benchmark_neuralnet neuralnet)
