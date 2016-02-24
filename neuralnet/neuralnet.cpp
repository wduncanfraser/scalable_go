// Implementation of Neural Network Class with arbitrary layers
// Author: W. Duncan Fraser
// Email: duncan@wduncanfraser.com

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>

#include "neuralnet.h"

inline double activate(double x) {
    // Using
    return ((x) / (1 + std::abs(x)));
}

NeuralNet::NeuralNet() {

}

NeuralNet::NeuralNet(const unsigned int i_layer_count, const std::vector<unsigned int> i_neuron_counts) {
    // Check that layer count is correct
    if (i_layer_count == i_neuron_counts.size()) {
        layer_count = i_layer_count;
        neuron_counts = i_neuron_counts;
    } else {
        // Error generation
        std::cout << "Layer Count does not match input neuron counts.\n";
    }
    // Resize Neuron and Weight vectors to match layer_count
    neurons.resize(layer_count);
    weights.resize(layer_count - 1);

    // Assign/Resize Neuron Vectors to size of neural network layers and initialize to 0.
    // Input and hidden layers +1 to account for bias
    for (unsigned int i = 0; i < layer_count; i++) {
        // If output layer, don't add bias
        if (i == (layer_count - 1)) {
            neurons[i].assign(neuron_counts[i], 0);
        } else {
            // For all other layers, add bias
            neurons[i].assign(neuron_counts[i] + 1, 0);
            // Set the bias to 1
            neurons[i][neuron_counts[i]] = 1.0;
        }
    }

    // Assign/Resize Weight Vectors to appropriate size [x][y] and initialize elements to 0.
    // +1 is to account for bias
    for (unsigned int i = 1; i < layer_count; i++) {
        weights[i - 1].resize(neuron_counts[i]);

        for (std::vector<double> &row : weights[i - 1]) {
            row.assign(neuron_counts[i - 1] + 1, 0);
        }
    }
}

NeuralNet::NeuralNet(const NeuralNet &i_network) {
    // Copy layer count
    layer_count = i_network.layer_count;
    // Copy layer neuron counts
    neuron_counts = i_network.neuron_counts;
    // Copy Neurons
    neurons = i_network.neurons;
    // Copy Weights
    weights = i_network.weights;
}

NeuralNet::~NeuralNet() {
    // std::cout << "Running Destructor";
}

NeuralNet &NeuralNet::operator=(const NeuralNet &i_network) {
    if (this != &i_network) {
        // Copy layer count
        layer_count = i_network.layer_count;
        // Copy layer neuron counts
        neuron_counts = i_network.neuron_counts;
        // Copy Neurons
        neurons = i_network.neurons;
        // Copy Weights
        weights = i_network.weights;
    }
    return *this;
}

bool NeuralNet::operator==(const NeuralNet &i_network) const {
    return (weights == i_network.weights);
}

bool NeuralNet::operator!=(const NeuralNet &i_network) const {
    return (weights != i_network.weights);
}

void NeuralNet::initialize_random() {
    // Setup random number generator
    std::random_device rd;
    // Generate random number as seed for twister engine
    std::mt19937 generator(rd());
    // Set bounds for real distribution
    std::uniform_real_distribution<double> distribution(-1.0, 1.0);

    // Assign random values to each element in each row of weight table
    for (std::vector<std::vector<double>> &layer : weights) {
        for (std::vector<double> &row : layer) {
            for (double &element : row) {
                element = distribution(generator);
            }
        }
    }
}

int NeuralNet::export_weights_stream(std::ofstream &file, bool clean) {
    if (file.is_open()) {
        DoubleInt converter;
        if (!clean) {
            for (unsigned int i = 0; i < layer_count; i++) {
                file << "Layer " << i << " count: " << neuron_counts[i] << std::endl;
            }
        } else {
            file << layer_count << std::endl;

            for (unsigned int i = 0; i < layer_count; i++) {
                file << neuron_counts[i] << ",";
            }
            file << std::endl;
        }

        for (unsigned int i = 0; i < layer_count - 1; i++) {
            if (!clean) {
                std::cout << "\nLayer " << i << " to layer " << i + 1 << " weights: \n";
            }

            for (std::vector<double> &row : weights[i]) {
                for (double &element : row) {
                    converter.d = element;
                    file << converter.i << ",";
                }
            }
        }
        // Newline for parsing on import
        file << std::endl;

        return 0;
    } else {
        return 1;
    }
}

int NeuralNet::import_weights_stream(std::ifstream &file) {
    if (!file.is_open()) {
        std::cout << "File is not open for import_weights_stream.\n";
        return 1;
    }

    // Import weight values
    unsigned int import_layer_count;
    std::vector<unsigned int> import_layer_neuron_count;
    // Vector to hold imported values
    std::vector<std::vector<std::vector<double>>> import_weights;

    // String and stringstream for converting data
    std::string line;

    std::istringstream layer_stream, weight_stream;

    // Get layer count
    getline(file, line, '\n');
    import_layer_count = std::stoi(line);

    if (import_layer_count != layer_count) {
        std::cout << "Import layer count does not match.\n";
        return 2;
    }

    import_weights.resize(layer_count - 1);

    // Get layer neuron counts
    getline(file, line, '\n');
    layer_stream.str(line);

    // string to hold line elements
    std::string line_element;

    while (layer_stream) {
        if (!getline(layer_stream, line_element, ',')) {
            break;
        }

        import_layer_neuron_count.push_back(std::stoi(line_element));
    }

    // Check that count matches layer count, malformed otherwise
    if (import_layer_neuron_count.size() != layer_count) {
        return 10;
    }

    // Check that neuron counts match
    if (import_layer_neuron_count != neuron_counts) {
        return 3;
    }

    // DoubleInt Union converter for importing doubles stored as ints
    DoubleInt converter;

    // Assign/Reize Weight Vectors to appropriate size [x][y] and initialize elements to 0.
    // +1 is to account for bias
    for (unsigned int i = 1; i < layer_count; i++) {
        import_weights[i - 1].resize(neuron_counts[i]);

        for (std::vector<double> &row : import_weights[i - 1]) {
            row.assign(neuron_counts[i - 1] + 1, 0);
        }
    }

    // Get line with weights
    getline(file, line, '\n');
    weight_stream.str(line);

    // Import Weights
    for (unsigned int i = 0; i < layer_count - 1; i++) {
        for (std::vector<double> &row : import_weights[i]) {
            for (double &element : row) {
                if (!getline(weight_stream, line_element, ',')) {
                    // Malformed, ran out of input
                    return 10;
                }
                converter.i = std::stoll(line_element);
                element = converter.d;
            }
        }
    }

    // Convert imported values to weight vectors
    weights = import_weights;

    return 0;
}

int NeuralNet::feed_forward(const std::vector<double> &input) {
    // If the size of the input vector is not the same as the Neural Network input layer, return error code.
    if (input.size() != neuron_counts[0]) {
        return 1;
    } else {
        // Assign input_Neurons to match input. Last element of input_Neurons should not be touched as it is bias.
        // For each loops not being used throughout function to avoid bias nuerons and needing iterators for
        // accessing multiple vectors.
        for (unsigned int i = 0; i < neuron_counts[0]; i++) {
            neurons[0][i] = input[i];
        }

        // Calculate through all layers
        for (unsigned int i = 1; i < layer_count; i++) {
            // For each neuron in each layer after input, calculate value
            for (unsigned int j = 0; j < neuron_counts[i]; j++) {
                // Reset Nueron to 0 before calculating
                neurons[i][j] = 0;

                // Sum the value of all weights*previous layer neuron value
                for (unsigned int k = 0; k <= neuron_counts[i - 1]; k++) {
                    neurons[i][j] += neurons[i - 1][k] * weights[i - 1][j][k];
                }

                // Take activate of final sum to determine value
                neurons[i][j] = activate(neurons[i][j]);
            }
        }

        // Finished executing without error, return 0;
        return 0;
    }
}

int NeuralNet::mutate(const double &radius) {
    // Setup random number generator
    std::random_device rd;
    // Generate random number as seed for twister engine
    std::mt19937 generator(rd());
    // Set bounds for real distribution
    std::uniform_real_distribution<double> distribution(-radius, radius);

    // Mutate each element in each row of weight tables by random values in uniform distribution
    for (std::vector<std::vector<double>> &layer : weights) {
        for (std::vector<double> &row : layer) {
            for (double &element : row) {
                element += distribution(generator);
            }
        }
    }

    // Finished executing without error, return 0;
    return 0;
}

std::vector<double> NeuralNet::get_output() const {
    return neurons[layer_count - 1];
}
