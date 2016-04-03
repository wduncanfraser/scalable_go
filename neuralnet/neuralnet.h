// Copyright [2015, 2016] <duncan@wduncanfraser.com>
// Prototype of Neural Network class with arbitrary layers

#ifndef NEURALNET_NEURALNET_H_
#define NEURALNET_NEURALNET_H_

#include <vector>
#include <fstream>
#include <cmath>

class NeuralNetFeedForwardError : public std::runtime_error {
 public:
    NeuralNetFeedForwardError() : std::runtime_error("NeuralNetFeedForwardError") { }
};

class NeuralNetExportError : public std::runtime_error {
 public:
    NeuralNetExportError() : std::runtime_error("NeuralNetExportError") { }
};

class NeuralNetImportError : public std::runtime_error {
 public:
    NeuralNetImportError() : std::runtime_error("NeuralNetImportError") { }
};

// Support function for calculating activate.
// Declared inline as it is only 1 line to increase speed.
inline double activate(double x) {
    return ((x) / (1 + std::abs(x)));
}

// Union for storing weights due to rounding
union DoubleInt {
    int64_t i;
    double d;
};

// NeuralNet class definition
class NeuralNet {
 private:
    // Number of layers. Must be at least 2.
    unsigned int layer_count;
    // Number on Neurons for each layer
    std::vector<unsigned int> neuron_counts;
    // Neurons
    std::vector<std::vector<double>> neurons;
    // Weights
    std::vector<std::vector<std::vector<double>>> weights;

 public:
    // Default Constructor
    NeuralNet();

    // Constructor with size specifications for layers.
    NeuralNet(const unsigned int i_layer_count, const std::vector<unsigned int> i_neuron_counts);

    // Copy Constructor
    NeuralNet(const NeuralNet &i_network);

    // Destructor
    ~NeuralNet();

    // Copy operator
    NeuralNet &operator=(const NeuralNet &i_network);

    // Comparison Operator
    bool operator==(const NeuralNet &i_network) const;

    // Inequality Operator
    bool operator!=(const NeuralNet &i_network) const;

    // Initialize Neural Network with random weights in a uniform distribution
    void initialize_random();

    // FeedForward Function, calculate output based on inputs.
    void feed_forward(const std::vector<double> &input);

    // Mutator. Randomly mutates
    void mutate(const double &radius);

    // Get output
    std::vector<double> get_output() const;

    // Export weights to specified ofstream
    void export_weights_stream(std::ofstream &file);

    // Import weights from specified file. Each weight set needs to be CSV on a single line
    void import_weights_stream(std::ifstream &file);
};

#endif  // NEURALNET_NEURALNET_H_
