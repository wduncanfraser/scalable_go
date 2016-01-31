cmake_minimum_required(VERSION 2.8)

project(goboard)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")

set(HEADER_FILES
        goboard.h
        )

set(SOURCE_FILES
        goboard.cpp
        )

add_library(goboard STATIC ${SOURCE_FILES} ${HEADER_FILES})
