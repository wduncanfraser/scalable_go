// Copyright [2015, 2016] <duncan@wduncanfraser.com>
// Performance test for NeuralNet

#include <iostream>
#include <chrono>
#include <vector>
#include <limits>

#include "gogame.h"
#include "gogameab.h"
#include "gogamenn.h"

int main() {
    uint8_t board_size = 19;
    GoGame test_game(board_size);
    GoMove best_move(test_game.get_board());

    std::chrono::time_point<std::chrono::system_clock> start, end;

    // Setup test NN
    GoGameNN test_network(board_size, false);
    test_network.initialize_random();

    // Value of best move
    double best_move_value, temp_best_move_value = 0;

    // Start timing for ab prune
    start = std::chrono::system_clock::now();

    // Generate and take black move
    test_game.generate_moves(0);
    best_move_value = -std::numeric_limits<double>::infinity();

    std::vector<GoMove> move_list = test_game.get_move_list();
    // For each possible move, calculate Alpha Beta
    #pragma omp parallel for firstprivate(test_network)
    for (unsigned int i = 0; i < move_list.size(); i++) {
        GoGame temp_game(test_game);
        temp_game.make_move(move_list[i], 0);

        temp_best_move_value = scalable_go_ab_prune(test_network, temp_game, 0,
                                                    -std::numeric_limits<double>::infinity(),
                                                    std::numeric_limits<double>::infinity(), 1, false, 0);

        if (temp_best_move_value > best_move_value) {
            best_move_value = temp_best_move_value;
            best_move = move_list[i];
        }
    }
    // Make Black Move
    test_game.make_move(best_move, 0);

    // End timing
    end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time evaluating worst case 19x19 move with 0ply: " << elapsed_seconds.count() << "s\n";
}
