#!/usr/bin/env bash

# Make bin directory and cd
mkdir build || true
cd build

# Build project
cmake ..

# Run Tests
make
./neuralnet_tests
