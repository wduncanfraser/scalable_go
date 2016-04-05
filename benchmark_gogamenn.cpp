// Copyright [2015, 2016] <duncan@wduncanfraser.com>
// Performance test for NeuralNet

#include <iostream>
#include <chrono>
#include <vector>

#include "gogamenn.h"

#define BOARD_SIZE 9;
#define ITERATIONS 1000000;

class BenchmarkArgumentError : public std::runtime_error {
 public:
    BenchmarkArgumentError() : std::runtime_error("BenchmarkArgumentError") { }
};

int main(int argc, char* argv[]) {
    uint8_t board_size = 0;
    uint32_t iterations = 0;

    // Validate command line parameters
    if (argc == 1) {
        // No parameters, use the Macros
        board_size = BOARD_SIZE;
        iterations = ITERATIONS;
    } else if (argc == 3) {
        // TODO(wdfraser): Add some better error checking
        board_size = uint8_t(atoi(argv[1]));
        iterations = atoi(argv[2]);
    } else {
        throw BenchmarkArgumentError();
    }

    GoGame test_game(board_size);
    std::vector<std::vector<double>> networks_translation = get_go_network_translation(test_game, 0);

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    GoGameNN test1(board_size, false);
    test1.initialize_random();

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time initializing NeuralNet: " << elapsed_seconds.count() << "s\n";


    // Start timing for feedforward
    start = std::chrono::system_clock::now();
    #pragma omp parallel for firstprivate(test1)
    for (unsigned int i = 0; i < iterations; i++) {
        test1.feed_forward(networks_translation, test_game.get_pieces_placed()[0], test_game.get_prisoner_count()[0],
                           test_game.get_prisoner_count()[1]);
    }
    // End timing
    end = std::chrono::system_clock::now();

    elapsed_seconds = end - start;
    std::cout << "Elapsed time evaluating " << iterations << " iterations: " << elapsed_seconds.count() << "s\n";
    std::cout << "Iterations per second: " << iterations / elapsed_seconds.count() << std::endl;
}
