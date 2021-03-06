// Copyright [2015, 2016] <duncan@wduncanfraser.com>

#include <array>
#include <vector>
#include <iostream>
#include <string>
#include <limits>
#include <stdexcept>
#include <chrono>
#include <random>

#include "gogame.h"
#include "gogamenn.h"
#include "gogameab.h"

#define DEPTH 1
#define MUTATER 0.01

#define NETWORKCOUNT 30
#define NETWORKKEEP 10

#define BOARD_SIZE 3
#define TRAINING_SET 1
#define STARTCYCLE 1
#define ENDCYCLE 200
#define UNIFORM 0
#define SCALED 0

class TrainingArgumentError : public std::runtime_error {
 public:
    TrainingArgumentError() : std::runtime_error("TrainingArgumentError") { }
};

class TrainingImportError : public std::runtime_error {
public:
    TrainingImportError() : std::runtime_error("TrainingImportError") { }
};

std::vector<int> score_networks(std::vector<GoGameNN> networks, const uint8_t board_size) {
    // Vector to hold win counts for networks
    std::vector<int> scores(networks.size(), 0);

    // Each network plays every other network as each team, storing total score for each neural network.
    #pragma omp parallel for firstprivate(networks) schedule(dynamic, 1)
    for (unsigned int i = 0; i < networks.size(); i++) {
        for (unsigned int j = 0; j < networks.size(); j++) {
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

                    temp_best_move_value = scalable_go_ab_prune(networks[i], temp_game, DEPTH,
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

                    temp_best_move_value = scalable_go_ab_prune(networks[j], temp_game, DEPTH,
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
                        scores[i] += 1;
                        scores[j] -= 1;
                    } else if (game_score[1] > game_score[0]) {
                        // White wins
                        scores[j] += 1;
                        scores[i] -= 1;
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
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    std::cout << "Started computation at " << std::ctime(&start_time) << std::endl;


    uint8_t board_size = 0;
    unsigned int training_set = 0;
    unsigned int start_cycle = 0;
    unsigned int end_cycle = 0;
    bool uniform = 0;
    bool scaled = 0;
    // Validate command line parameters
    if (argc == 1) {
        // No parameters, use the Macros
        board_size = BOARD_SIZE;
        training_set = TRAINING_SET;
        start_cycle = STARTCYCLE;
        end_cycle = ENDCYCLE;
        uniform = UNIFORM;
        scaled = SCALED;
    } else if (argc == 7) {
        // TODO(wdfraser): Add some better error checking
        board_size = uint8_t(atoi(argv[1]));
        training_set = atoi(argv[2]);
        start_cycle = atoi(argv[3]);
        end_cycle = atoi(argv[4]);
        uniform = atoi(argv[5]) != 0;
        scaled = atoi(argv[6]) != 0;
    } else {
        throw TrainingArgumentError();
    }

    for (unsigned int n = start_cycle; n <= end_cycle; n++) {
        std::vector<GoGameNN> training_networks(NETWORKCOUNT, GoGameNN(board_size, uniform));
        std::vector<int> training_scores(NETWORKCOUNT);

        // Scaling networks used for seeding networks if scaled = true
        std::vector<GoGameNN> scaling_networks;
        // Set Random generator for use when selecting networks for reseeding
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, NETWORKKEEP - 1);

        unsigned int export_count = 0;

        std::string output_directory =
                "size" + std::to_string(board_size) + "set" + std::to_string(training_set) + "/";

        std::ifstream best_networks_in(output_directory + "lastbestnetworks.txt");
        std::ifstream import_networks(output_directory + "import_networks.txt");

        if (scaled) {
            // If scaled network, confirm import_networks is open
            if (!import_networks.is_open()) {
                throw TrainingImportError();
            } else {
                scaling_networks.assign(NETWORKKEEP, GoGameNN(board_size - SEGMENT_DIVISION, uniform));
                // If it is, import example_networks;
                for (unsigned int i = 0; i < NETWORKKEEP; i++) {
                    scaling_networks[i].import_weights_stream(import_networks);
                }
            }
        }

        std::cout << "Generation with " << NETWORKCOUNT << " Neural Networks.\n"
        << "Total Games: " << NETWORKCOUNT * NETWORKCOUNT << std::endl;

        if (best_networks_in.is_open()) {
            std::cout << "Starting generation " << n << ". Last best network file succesfully opened. \n";

            // Read kept networks from file
            for (unsigned int i = 0; i < NETWORKKEEP; i++) {
                training_networks[i].import_weights_stream(best_networks_in);
            }
            for (unsigned int i = 0; i < NETWORKKEEP; i++) {
                training_networks[i + NETWORKKEEP] = training_networks[i];
                training_networks[i + NETWORKKEEP].mutate(MUTATER);
            }
            for (unsigned int i = 0; i < NETWORKKEEP; i++) {
                if (scaled) {
                    // If scaled network, seed network subsections from a random imported network
                    training_networks[i + (NETWORKKEEP * 2)].scale_network(scaling_networks[dis(gen)]);
                } else {
                    training_networks[i + (NETWORKKEEP * 2)].initialize_random();
                }
            }
        } else if (n == 1) {
            std::cout << "Starting first generation. Last best network file failed to open."
            << " Initializing random weights. \n";
            for (GoGameNN &element : training_networks) {
                if (scaled) {
                    // If scaled network, seed network subsections from a random imported network
                    element.scale_network(scaling_networks[dis(gen)]);
                } else {
                    element.initialize_random();
                }
            }
        } else {
            std::cout << "Starting generation " << n << ". Last best network file failed to open. Ending training. \n";
            break;
        }

        training_scores = score_networks(training_networks, board_size);

        for (unsigned int i = 0; i < training_scores.size(); i++) {
            std::cout << "Neural Network: " << i << ". Score: " << training_scores[i] << ".\n";
        }

        std::ofstream output_file(output_directory + "generation" + std::to_string(n) + ".txt");
        std::ofstream best_networks_file(output_directory + "lastbestnetworks.txt",
                                         std::ofstream::out | std::ofstream::trunc);

        if (output_file.is_open()) {
            for (unsigned int i = 0; i < training_scores.size(); i++) {
                output_file << "Neural Network: " << i << ". Score: " << training_scores[i] << ".\n";
            }
            output_file << std::endl;
            for (unsigned int i = 0; i < training_scores.size(); i++) {
                output_file << "Neural Network: " << i << ".\n";
                training_networks[i].export_weights_stream(output_file);
                output_file << std::endl;
            }
            output_file.close();
        } else {
            std::cout << "Error opening output file. \n";
            break;
        }

        if (best_networks_file.is_open()) {
            // Check down to lowest possible score, -NETWORKCOUNT*2
            for (int i = NETWORKCOUNT - 1; i > -NETWORKCOUNT * 2; i--) {
                if (export_count >= NETWORKKEEP) {
                    break;
                }
                for (unsigned int j = 0; j < NETWORKCOUNT; j++) {
                    if (export_count >= NETWORKKEEP) {
                        break;
                    }
                    if (training_scores[j] == i) {
                        training_networks[j].export_weights_stream(best_networks_file);
                        export_count += 1;
                    }
                }
            }
            best_networks_file.close();
        } else {
            std::cout << "Error opening best networks file. \n";
            break;
        }
    }
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "Finished computation at " << std::ctime(&end_time) << "elapsed time: " <<
            elapsed_seconds.count() << "s" << std::endl;
}
