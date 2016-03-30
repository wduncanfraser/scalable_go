cmake_minimum_required(VERSION 2.8)

project(gogame)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")

set(HEADER_FILES
        gogame.h
        )

set(SOURCE_FILES
        gogame.cpp
        )

add_library(gogame STATIC ${SOURCE_FILES} ${HEADER_FILES})
