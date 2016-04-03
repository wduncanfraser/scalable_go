cmake_minimum_required(VERSION 2.8)

project(gogameab)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()

set(HEADER_FILES
        gogameab.h
        )

set(SOURCE_FILES
        gogameab.cpp
        )

add_library(gogameab STATIC ${SOURCE_FILES} ${HEADER_FILES})
