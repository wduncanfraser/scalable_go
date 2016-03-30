cmake_minimum_required(VERSION 2.8)

project(gogame)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")

set(HEADER_FILES
        ab_prune.h
        )

set(SOURCE_FILES
        ab_prune.cpp
        )

add_library(ab_prune STATIC ${SOURCE_FILES} ${HEADER_FILES})
