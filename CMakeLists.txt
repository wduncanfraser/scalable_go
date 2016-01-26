cmake_minimum_required(VERSION 2.8)
project(Scalable_Go)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/")

set(SOURCE_LIBS
    lib/neuralnet.cpp)

set(SOURCE_NET_TEST
    test_neuralnet.cpp)

add_executable(test_neuralnet ${SOURCE_LIBS} ${SOURCE_NET_TEST})
