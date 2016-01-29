cmake_minimum_required(VERSION 2.8)

project(goboard)

add_definitions(-std=c++11)

set(HEADER_FILES
        goboard.h
        )

set(SOURCE_FILES
        goboard.cpp
        )

add_library(goboard STATIC ${SOURCE_FILES} ${HEADER_FILES})
