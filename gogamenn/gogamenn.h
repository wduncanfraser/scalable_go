// Copyright [2016] <duncan@wduncanfraser.com>
// Prototypes for Scalable Go AB prune algorithm and helper classes/functions

#ifndef GOGAMENN_GOGAMENN_H_
#define GOGAMENN_GOGAMENN_H_

#include <vector>
#include <cstdint>
#include <stdexcept>

#include "neuralnet.h"
#include "gogame.h"

#define SEGMENT_MIN 3
#define SEGMENT_MAX 19
#define SEGMENT_DIVISION 2

// GoGameNN exceptions
class GoGameNNSegmentError : public std::runtime_error {
 public:
    GoGameNNSegmentError() : std::runtime_error("GoGameNNSegmentError") { }
};

class GoGameNNInitError : public std::runtime_error {
 public:
    GoGameNNInitError() : std::runtime_error("GoGameNNInitError") { }
};

class GoGameNNFeedForwardError : public std::runtime_error {
public:
    GoGameNNFeedForwardError() : std::runtime_error("GoGameNNFeedForwardError") { }
};

class GoGameNNScaleError : public std::runtime_error {
 public:
    GoGameNNScaleError() : std::runtime_error("GoGameNNScaleError") { }
};

// Function that returns a vector of all segment sizes for a board of specified size.
// The specified size must be a valid segment size.
std::vector<uint8_t> get_go_board_segments(const uint8_t board_size);

// Function for taking the current board state of an instance of GoGame and serializing it for GoGameNN consumption.
// Blank spaces return as 0, friendly pieces return as 1, enemy pieces return as -1
std::vector<std::vector<double>> get_go_network_translation(const GoGame &i_gogame, const bool color);


// Class for holding a GoGame neuralnet. Wrapper around NeuralNet
class GoGameNN {
 private:
    // Bool to store wether GoGameNN is a uniform network
    bool uniform;

    // Board size
    uint8_t board_size;

    // Vector holding first layer of neural nets
    std::vector<NeuralNet> layer1;

    // Second layer neural net
    NeuralNet layer2;

 public:
    // Constructor with size specification
    GoGameNN(const uint8_t i_board_size, const bool i_uniform);

    // Copy Constructor
    GoGameNN(const GoGameNN &i_network);

    // Comparison Operator
    bool operator==(const GoGameNN &i_network) const;

    // Initialize Neural Networks with random weights in a uniform distribution
    void initialize_random();

    // Mutator. Randomly mutates using a uniform distribution
    void mutate(const double &radius);

    // FeedForward Function, calculate output based on inputs.
    void feed_forward(const std::vector<std::vector<double>> &input_segments, const uint8_t pieces_played,
                      const uint8_t prisoner_count, const uint8_t opponent_prisoner_count);

    // Get output
    const double get_output() const;

    // Export weights to specified ofstream. Wrapper around NeuralNet::expot_weights_stream
    void export_weights_stream(std::ofstream &file);

    // Import weights from specified ifstream. Wrapper around NeuralNet::import_weights_stream
    void import_weights_stream(std::ifstream &file);

    // Import weights from an existing network and scale up. New sections are initialized randomly.
    void scale_network(const GoGameNN &i_network);

    // Function to retrieve layer 1 networks. Used in testing
    std::vector<NeuralNet> get_layer1();
};

#endif  // GOGAMENN_GOGAMENN_H_
