// Copyright [2016] <duncan@wduncanfraser.com>
// Prototypes for Scalable Go AB prune algorithm and helper classes/functions

#ifndef AB_PRUNE_AB_PRUNE_H_
#define AB_PRUNE_AB_PRUNE_H_

#include <vector>
#include <cstdint>
#include <stdexcept>

#include "neuralnet.h"
#include "gogame.h"

#define SEGMENT_MIN 3
#define SEGMENT_MAX 9
#define SEGMENT_DIVISION 2

// ABPrune exceptions
class ABPruneSegmentError : public std::runtime_error {
 public:
    ABPruneSegmentError() : std::runtime_error("ABPruneSegmentError") { }
};

class ABPruneNetworkInitError : public std::runtime_error {
 public:
    ABPruneNetworkInitError() : std::runtime_error("ABPruneNetworkInitError") { }
};

// Function that returns a vector of all segment sizes for a board of specified size.
// The specified size must be a valid segment size.
std::vector<uint8_t> get_board_segments(const uint8_t board_size);

// Function for taking the current board state of an instance of GoGame and serializing it for ab_prune consumption.
// Blank spaces return as 0, friendly pieces return as 1, enemy pieces return as -1
std::vector<std::vector<double>> scalable_go_network_translation(const GoGame &i_gogame, const bool color);

// Alpha Beta Pruning algorithm for Go move generation
double scalable_go_ab_prune(NeuralNet &network, const GoGame &i_gogame, const int depth, double alpha, double beta,
                            const bool move_color, const bool max_player, const bool player_color);


// Class for holding a GoGame neuralnet. Wrapper around NeuralNet
class GoBoardNeuralNet {
 private:
    // Vector holding first layer of neural nets
    std::vector<NeuralNet> layer1;

    // Second layer neural net
    NeuralNet layer2;

 public:
    // Constructor with size specification
    explicit GoBoardNeuralNet(const uint8_t board_size);

    // Copy Constructor
    GoBoardNeuralNet(const GoBoardNeuralNet &i_network);

    // Comparison Operator
    bool operator==(const GoBoardNeuralNet &i_network) const;

    // Destructor
    virtual ~GoBoardNeuralNet();

    // FeedForward Function, calculate output based on inputs.
    // Returns 0 if no error
    // Returns 1 if vector size does not match neural network input layer
    int feed_forward(const std::vector<std::vector<double>> &input);

    // Get output
    const double get_output() const;
};

#endif  // AB_PRUNE_AB_PRUNE_H_
