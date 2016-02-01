#!/usr/bin/env bash

TRAVIS_OS_NAME=$1
CXX=$2

# Make bin directory and cd
mkdir build || true
cd build

# Build project
cmake ..

# Run Tests
if [ "$CXX" = "g++" ] && [ "$TRAVIS_OS_NAME" = "linux" ]; then
    ./configure --enable-gcov && make && make check
else
    make
fi

./neuralnet_tests
./goboard_tests