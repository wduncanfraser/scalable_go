// Copyright [2015, 2016] <duncan@wduncanfraser.com>

#include <array>
#include <vector>
#include <iostream>
#include <string>
#include <limits>
#include <stdexcept>

#include "gogame.h"
#include "gogamenn.h"
#include "gogameab.h"

#define DEPTH 1

#define NETWORKKEEP 10

class ComparisonArgumentError : public std::runtime_error {
 public:
    ComparisonArgumentError() : std::runtime_error("ComparisonArgumentError") { }
};

class ComparisonImportError : public std::runtime_error {
 public:
    ComparisonImportError() : std::runtime_error("ComparisonImportError") { }
};

std::array<std::vector<int>, 2> compare_sets(std::vector<GoGameNN> i_set1, std::vector<GoGameNN> i_set2, const uint8_t board_size) {
    // Array of Vectors to hold win counts for networks
    std::array<std::vector<int>, 2> scores;
    scores[0].assign(NETWORKKEEP, 0);
    scores[1].assign(NETWORKKEEP, 0);

    // Each network plays every network from the opposing set, storing total score for each neural network.
    // Set 1 always plays as black. Set 2 always plays as white.
    #pragma omp parallel for firstprivate(i_set1, i_set2) schedule(dynamic, 1)
    for (unsigned int i = 0; i < NETWORKKEEP; i++) {
        for (unsigned int j = 0; j < NETWORKKEEP; j++) {
            // Play as one team, then swap

            // GoGame instance used for training matches
            GoGame training_game(board_size);

            GoMove best_move(training_game.get_board());

            // Bool to determine if game should continue
            bool continue_match = true;

            // Value of best move
            double best_move_value, temp_best_move_value = 0;


            while (continue_match) {
                // Generate and take black move
                training_game.generate_moves(0);
                best_move_value = -std::numeric_limits<double>::infinity();

                // For each possible move, calculate Alpha Beta
                for (const GoMove &element : training_game.get_move_list()) {
                    GoGame temp_game(training_game);
                    temp_game.make_move(element, 0);

                    temp_best_move_value = scalable_go_ab_prune(i_set1[i], temp_game, DEPTH,
                                                                -std::numeric_limits<double>::infinity(),
                                                                std::numeric_limits<double>::infinity(), 1, false, 0);

                    if (temp_best_move_value > best_move_value) {
                        best_move_value = temp_best_move_value;
                        best_move = element;
                    }
                }
                // Make Black Move
                training_game.make_move(best_move, 0);

                // Generate and take white move
                training_game.generate_moves(1);
                best_move_value = -std::numeric_limits<double>::infinity();

                // For each possible move, calculate Alpha Beta
                for (const GoMove &element : training_game.get_move_list()) {
                    GoGame temp_game(training_game);
                    temp_game.make_move(element, 1);

                    temp_best_move_value = scalable_go_ab_prune(i_set2[j], temp_game, DEPTH,
                                                                -std::numeric_limits<double>::infinity(),
                                                                std::numeric_limits<double>::infinity(), 0, false, 1);

                    if (temp_best_move_value > best_move_value) {
                        best_move_value = temp_best_move_value;
                        best_move = element;
                    }
                }
                // Make White move
                training_game.make_move(best_move, 1);

                // Game end detection
                std::vector<GoMove> history(training_game.get_move_history());
                // Check if the last 2 moves were passes. If so, end
                if (history[history.size() - 1].check_pass() && history[history.size() - 2].check_pass()) {
                    std::array<uint8_t, 2> game_score = training_game.calculate_scores();

                    if (game_score[0] > game_score[1]) {
                        // Black Wins
                        scores[0][i] += 1;
                        scores[1][j] -= 1;
                    } else if (game_score[1] > game_score[0]) {
                        // White wins
                        scores[1][j] += 1;
                        scores[0][i] -= 1;
                    }
                    // Else, draw... assign no scores.
                    continue_match = false;
                }
            }
        }
    }
    return scores;
}

int main(int argc, char* argv[]) {
    uint8_t board_size = 0;
    std::string set_1_dir = "";
    bool set_1_uniform = 0;
    std::string set_2_dir = "";
    bool set_2_uniform = 0;

    // Validate command line parameters
    if (argc == 6) {
        // TODO(wdfraser): Add some better error checking
        board_size = uint8_t(atoi(argv[1]));
        set_1_dir = argv[2];
        set_1_uniform = atoi(argv[3]) != 0;
        set_2_dir = argv[4];
        set_2_uniform = atoi(argv[5]) != 0;
    } else {
        throw ComparisonArgumentError();
    }

    std::vector<GoGameNN> set_1_networks(NETWORKKEEP, GoGameNN(board_size, set_1_uniform));
    std::vector<GoGameNN> set_2_networks(NETWORKKEEP, GoGameNN(board_size, set_2_uniform));

    // Import networks
    std::ifstream set_1_in(set_1_dir + "/lastbestnetworks.txt");
    std::ifstream set_2_in(set_2_dir + "/lastbestnetworks.txt");

    // Import set 1
    if (set_1_in.is_open()) {
        for (unsigned int i = 0; i < NETWORKKEEP; i++) {
            set_1_networks[i].import_weights_stream(set_1_in);
        }
    } else {
        std::cout << "Set 1 file failed to open. Ending comparison. \n";
        throw ComparisonImportError();
    }

    // Import set 2
    if (set_2_in.is_open()) {
        for (unsigned int i = 0; i < NETWORKKEEP; i++) {
            set_2_networks[i].import_weights_stream(set_2_in);
        }
    } else {
        std::cout << "Set 2 file failed to open. Ending comparison. \n";
        throw ComparisonImportError();
    }

    std::cout << "Comparing set 1: " << set_1_dir << " against set 2: " << set_2_dir << std::endl;

    // Setup array to hold comparison scores
    std::array<std::vector<int>, 2> comparison_scores = compare_sets(set_1_networks, set_2_networks, board_size);
    std::array<std::vector<int>, 2> comparison_scores_reverse = compare_sets(set_2_networks, set_1_networks, board_size);

    for (unsigned int i = 0; i< NETWORKKEEP; i++) {
        comparison_scores[0][i] += comparison_scores_reverse[1][i];
        comparison_scores[1][i] += comparison_scores_reverse[0][i];
    }

    std::cout << "Final scores are as follows.\n";

    int set_1_total = 0;
    int set_2_total = 0;

    for (unsigned int i = 0; i < NETWORKKEEP; i++) {
        std::cout << "Set 1 Network " << i << ": " << comparison_scores[0][i] <<
                ". Set 2 Network " << i << ": " << comparison_scores[1][i] << ".\n";
        set_1_total += comparison_scores[0][i];
        set_2_total += comparison_scores[1][i];
    }

    // Set average

    std::cout << "Set 1 Network Average: " << set_1_total / NETWORKKEEP <<
    ". Set 2 Network Average: " << set_2_total / NETWORKKEEP << ".\n";
}
