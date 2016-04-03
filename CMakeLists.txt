cmake_minimum_required(VERSION 2.8.4)

project(Scalable_Go)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/")

set(NET_BENCHMARK
        benchmark_neuralnet.cpp)

set(GOGAMENN_BENCHMARK
        benchmark_gogamenn.cpp)

set(MOVESET_EXAMPLE
        basic_moveset.cpp)

set(TRAINING
        scalable_go_training.cpp)

add_executable(benchmark_neuralnet ${NET_BENCHMARK})

add_executable(benchmark_gogamenn ${GOGAMENN_BENCHMARK})

add_executable(basic_moveset ${MOVESET_EXAMPLE})

add_executable(scalable_go_training ${TRAINING})

include_directories(gogame neuralnet gogamenn gogameab)

add_subdirectory(gogame)
add_subdirectory(neuralnet)
add_subdirectory(gogamenn)
add_subdirectory(gogameab)
add_subdirectory(tests)

target_link_libraries(benchmark_neuralnet neuralnet)
target_link_libraries(benchmark_gogamenn neuralnet)
target_link_libraries(benchmark_gogamenn gogame)
target_link_libraries(benchmark_gogamenn gogamenn)
target_link_libraries(basic_moveset gogame)
target_link_libraries(scalable_go_training neuralnet)
target_link_libraries(scalable_go_training gogame)
target_link_libraries(scalable_go_training gogamenn)
target_link_libraries(scalable_go_training gogameab)
