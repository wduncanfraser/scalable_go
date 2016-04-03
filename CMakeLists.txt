cmake_minimum_required(VERSION 2.8.4)

project(Scalable_Go)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/")

set(NET_BENCHMARK
        benchmark_neuralnet.cpp)

set(MOVESET_EXAMPLE
        basic_moveset.cpp)

set(TRAINING
        scalable_go_training.cpp)

add_executable(benchmark_neuralnet ${NET_BENCHMARK})

add_executable(basic_moveset ${MOVESET_EXAMPLE})

add_executable(scalable_go_training ${TRAINING})

include_directories(gogame neuralnet gogamenn gogameab)

add_subdirectory(gogame)
add_subdirectory(neuralnet)
add_subdirectory(gogamenn)
add_subdirectory(gogameab)
add_subdirectory(tests)

target_link_libraries(benchmark_neuralnet neuralnet)
target_link_libraries(basic_moveset gogame)
target_link_libraries(scalable_go_training gogameab)