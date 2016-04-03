cmake_minimum_required(VERSION 2.8)

project(gogamenn)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")

set(HEADER_FILES
        gogamenn.h
        )

set(SOURCE_FILES
        gogamenn.cpp
        )

add_library(gogamenn STATIC ${SOURCE_FILES} ${HEADER_FILES})
