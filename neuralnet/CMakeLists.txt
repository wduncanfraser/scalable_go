cmake_minimum_required(VERSION 2.8)

project(neuralnet)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")

set(HEADER_FILES
        neuralnet.h
        )

set(SOURCE_FILES
        neuralnet.cpp
        )

add_library(neuralnet STATIC ${SOURCE_FILES} ${HEADER_FILES})
