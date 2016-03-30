#!/usr/bin/env bash

# Make bin directory and cd
mkdir build || true
cd build

# Build project
cmake ..
make

# Run Tests
./neuralnet_tests
./gogame_tests
./ab_prune_tests