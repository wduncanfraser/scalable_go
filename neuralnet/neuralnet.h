// Prototype of Neural Network class with arbitrary layers
// Author: W. Duncan Fraser
// Email: duncan@wduncanfraser.com

//ifdef to prevent multiple inclusions.
#ifndef NEURALNET_H
#define NEURALNET_H

//Support function for calculating activate.
//Declared inline as it is only 1 line to increase speed.
inline double activate(double x);

//Union for storing weights due to rounding
union DoubleInt {
    int64_t i;
    double d;
};

//NeuralNet class definition
class NeuralNet {
    //Variable declarations
private:
    //Number of layers. Must be at least 2.
    unsigned int layer_count;
    //Number on Neurons for each layer
    std::vector<unsigned int> neuron_counts;
    //Neurons
    std::vector<std::vector<double>> neurons;
    //Weights
    std::vector<std::vector<std::vector<double>>> weights;
public:
    //Default Constructor
    NeuralNet();

    //Constructor with size specifications for layers.
    NeuralNet(const unsigned int i_layer_count, const std::vector<unsigned int> i_neuron_counts);

    //Copy Constructor
    NeuralNet(const NeuralNet &i_network);

    //Destructor
    ~NeuralNet();

    //Copy operator
    NeuralNet &operator=(const NeuralNet &i_network);

    //Comparison Operator
    bool operator==(const NeuralNet &i_network) const;

    //Inequality OPerator
    bool operator!=(const NeuralNet &i_network) const;

    //Initialize Nueral Network with random weights in a uniform distribution
    void initialize_random();

    //Export weights to specified ofstream
    int export_weights_stream(std::ofstream &file, bool clean);

    //Import weights from specified file. Each weight set needs to be CSV on a single line
    // Returns 0 if no error and import succeeds
    // 1: Failed to open file
    // 2: Layer count doesn't match
    // 3: Neuron count doesn't match
    // 10: File malformed
    int import_weights_stream(std::ifstream &file);

    //FeedForward Function, calculate output based on inputs.
    // Returns 0 if no error
    // Returns 1 if vector size does not match neural network input layer
    int feed_forward(const std::vector<double> &input);

    //Mutator. Randomly mutates
    int mutate(const double &radius);

    //Get output
    std::vector<double> get_output() const;
};


#endif
