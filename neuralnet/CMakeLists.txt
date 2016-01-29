cmake_minimum_required(VERSION 2.8)

project(neuralnet)

add_definitions(-std=c++11)

set(HEADER_FILES
        neuralnet.h
        )

set(SOURCE_FILES
        neuralnet.cpp
        )

add_library(neuralnet STATIC ${SOURCE_FILES} ${HEADER_FILES})
