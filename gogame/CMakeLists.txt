cmake_minimum_required(VERSION 2.8)

project(gogame)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()

set(HEADER_FILES
        gogame.h
        gohelpers.h
        )

set(SOURCE_FILES
        gogame.cpp
        )

add_library(gogame STATIC ${SOURCE_FILES} ${HEADER_FILES})
