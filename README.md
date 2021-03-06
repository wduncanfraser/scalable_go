[![Build Status](https://travis-ci.org/wduncanfraser/scalable_go.svg?branch=master)](https://travis-ci.org/wduncanfraser/scalable_go)

# Scalable Go
## Purpose
CS Masters Project. Scaling evolutionary neural networks for Go artificial intelligence.

## Usage
### Training
+   Once compiled, create a directory in the same location as the binary called "size\<board size\>set\<set number\>". For example, "size3set1".
+   Run training with `./scalable_go_training <board_size> <set> <start_generation> <end_generation> <uniform> <scaled>`.
+   Uniform and scaled are booleans (enter 0 or 1) that determine if the network is uniform, and whether it is scaling up from a smaller network. If scaling up, "importnetworks.txt" much be present, which should be a copy of "lastbestnetworks.txt" from previous training on one size smaller board.

### Comparison
+   Run comparison with `./scalable_go_comparison <board_size> <set1_name> <set1_uniform> <set2_name> <set2_uniform>`. Example: `./scalable_go_comparison 5 size5set2 0 size5set6 1`
+   set1_uniform and set2_uniform are booleans (enter 0 or 1) that determine if the network is uniform.

### Benchmark
+   Run gogamenn benchmark with `./benchmark_gogamenn <board_size> <iterations>`. Benchmark will return total time to complete iterations and iterations per second.

## Structure
+   gogame/: Library for defining Go game, board, and move generation
+   neuralnet/: Neural Network Library
+   gogamenn/: Library defining NeuralNet wrapper for Go and helper functions.
+   gogameab/: Library defining AB Pruning algorithm.
+   tests/: Units and regression tests
+   benchmark_neuralnet.cpp: Basic benchmark of neural network performance.
+   benchmark_gogamenn.cpp: Basic benchmark of gogamenn performance.
+   benchmark_19x19ab_prune.cpp: Basic benchmark of worst case AB prune on 19x19 board with 0 ply.
+   scalable_go_comparison.cpp: Compares 2 sets of training results.
+   scalable_go_training.cpp: Training algorithm.

## Neuralnet Structure
### Layer 1 Subsection NeuralNet Node Counts
+ Input Layer: subsection size ^2, so 9 for a 3x3 subsection
+ HL1: Input Layer * (4/3) rounded down, so 12 for a 3x3 subsection
+ HL2: HL1 * (1/4) rounded down, so 3 for a 3x3 subsection
+ Output Layer: 1

## Layer 2 Neuralnet Nodes
+ Input Layer: Outputs from all Layer 1 networks, in addition to pieces placed, friendly, and opponent prisoner counts
+ HL1: Input Layer * (2/3) rounded down
+ Output Layer: 1

## External Resources
+   [Google Test](https://github.com/google/googletest)

## Legal
Author: [W. Duncan Fraser](duncan@wduncanfraser.com)

License: [Apache License V2](LICENSE)
