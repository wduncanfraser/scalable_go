// Copyright [2015, 2016] <duncan@wduncanfraser.com>
// Performance test for NeuralNet

#include <iostream>
#include <chrono>
#include <vector>

#include "gogamenn.h"

int main() {
    uint8_t board_size = 9;
    GoGame test_game(board_size);
    std::vector<std::vector<double>> networks_translation = get_go_network_translation(test_game, 0);

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    GoGameNN test1(board_size);
    test1.initialize_random();

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time initializing NeuralNet: " << elapsed_seconds.count() << "s\n";


    // Start timing for feedforward
    start = std::chrono::system_clock::now();
    #pragma omp parallel for firstprivate(test1)
    for (int i = 0; i < 1000000; i++) {
        test1.feed_forward(networks_translation, test_game.get_pieces_placed()[0], test_game.get_prisoner_count()[0],
                           test_game.get_prisoner_count()[1]);
    }
    // End timing
    end = std::chrono::system_clock::now();

    elapsed_seconds = end - start;
    std::cout << "Elapsed time evaluating 1,000,000 iterations: " << elapsed_seconds.count() << "s\n";
    std::cout << "Iterations per second: " << 1000000 / elapsed_seconds.count() << std::endl;
}
