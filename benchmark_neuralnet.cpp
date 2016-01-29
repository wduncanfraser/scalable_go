#include <iostream>
#include <chrono>
#include <vector>
#include <random>

//Test for NeuralNet
#include "neuralnet.h"

#define LAYERS 4
#define INPUT 32
#define HL1 40
#define HL2 10
#define OUTPUT 1
#define MUTATER 0.01

int main() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    NeuralNet test1(LAYERS, {INPUT, HL1, HL2, OUTPUT});
    test1.initializeRandomWeights();

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time initializing NeuralNet: " << elapsed_seconds.count() << "s\n";

    // Setup random number generator
    std::random_device rd;
    // Generate random number as seed for twister engine
    std::mt19937 generator(rd());
    // Set bounds for real distribution
    std::uniform_real_distribution<double> distribution(-1.0, 1.0);

    std::vector<double> testInput, testInput2;
    testInput.assign(INPUT, 0);

    for (double &element: testInput) {
        element = distribution(generator);
    }

    //Start timing for feedforward
    start = std::chrono::system_clock::now();
    //#pragma omp parallel for firstprivate(test1)
    for (int i = 0; i < 1000000; i++) {
        test1.feedForward(testInput);
    }
    // End timing
    end = std::chrono::system_clock::now();

    elapsed_seconds = end - start;
    std::cout << "Elapsed time evaluating 1,000,000 iterations: " << elapsed_seconds.count() << "s\n";
    std::cout << "Iterations per second: " << 1000000 / elapsed_seconds.count() << std::endl;
}
