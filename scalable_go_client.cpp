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
#include "gohelpers.h"

#define DEPTH 1

class ClientArgumentError : public std::runtime_error {
 public:
    ClientArgumentError() : std::runtime_error("ClientArgumentError") { }
};

class ClientImportError : public std::runtime_error {
 public:
    ClientImportError() : std::runtime_error("ClientImportError") { }
};

std::array<uint8_t, 2> play_game(GoGameNN &i_network, const uint8_t board_size) {
    // Array of Vectors to hold win counts for networks
    std::array<uint8_t , 2> scores = {0, 0};

    // Currently, networks plays as black. Player plays as white.
    // GoGame instance used for matches
    GoGame game(board_size);

    GoMove best_move(game.get_board());

    std::string x_in, y_in;

    // Bool to determine if game should continue
    bool continue_match = true;

    // Value of best move
    double best_move_value, temp_best_move_value = 0;


    while (continue_match) {
        std::cout << "Black taking move... \n";

        // Generate and take black move
        game.generate_moves(0);
        best_move_value = -std::numeric_limits<double>::infinity();

        // For each possible move, calculate Alpha Beta
        for (const GoMove &element : game.get_move_list()) {
            GoGame temp_game(game);
            temp_game.make_move(element, 0);

            temp_best_move_value = scalable_go_ab_prune(i_network, temp_game, DEPTH,
                                                        -std::numeric_limits<double>::infinity(),
                                                        std::numeric_limits<double>::infinity(), 1, false, 0);

            if (temp_best_move_value > best_move_value) {
                best_move_value = temp_best_move_value;
                best_move = element;
            }
        }
        // Make White move
        game.make_move(best_move, 0);

        // Check if passed
        if (best_move.check_pass()) {
            std::cout << "Black passed\n";
        } else {
            std::cout << "Black made move: (" << int(best_move.get_piece().x) << ", " << int(best_move.get_piece().y) <<
            ")\n";
        }

        // White is player move
        // Get player move
        while (true) {
            std::cout << "Current board state: \n";
            render_board(game.get_board());
            std::cout << "Enter coordinates for where you would like to move, or pass.\n";
            std::cout << "Enter X or pass: \n";
            std::cin >> x_in;
            std::cout << "Received " << x_in << std::endl;

            if (x_in == "pass") {
                best_move = GoMove(game.get_board());
            } else {
                std::cout << "Enter Y: \n";
                std::cin >> y_in;
                std::cout << "Received " << y_in << std::endl;

                try {
                    best_move = GoMove(game.get_board(), XYCoordinate(stoul(x_in), stoul(y_in)));
                } catch(...) {
                    std::cout << "Error creating move. Please try again.\n";
                    continue;
                }
            }

            // Attempt to make move.
            try {
                best_move.check_move(1);
                game.make_move(best_move, 1);
            } catch (...) {
                std::cout << "Invalid move, please try again.\n";
                continue;
            }

            break;
        }

        // Game end detection
        std::vector<GoMove> history(game.get_move_history());
        // Check if the last 2 moves were passes. If so, end
        if (history[history.size() - 1].check_pass() && history[history.size() - 2].check_pass()) {
            scores = game.calculate_scores();

            // Else, draw... assign no scores.
            continue_match = false;
        }
    }
    return scores;
}

int main(int argc, char* argv[]) {
    uint8_t board_size = 0;
    std::string network_file_path = "";
    bool network_uniform = 0;

    // Validate command line parameters
    if (argc == 4) {
        // TODO(wdfraser): Add some better error checking
        board_size = uint8_t(atoi(argv[1]));
        network_file_path = argv[2];
        network_uniform = atoi(argv[3]) != 0;
    } else {
        throw ClientArgumentError();
    }

    GoGameNN client_network(board_size, network_uniform);

    // Import network
    std::ifstream network_in(network_file_path);

    if (network_in.is_open()) {
        client_network.import_weights_stream(network_in);
    } else {
        std::cout << "Network file failed to open. Exiting. \n";
        throw ClientImportError();
    }

    std::cout << "Game start, Network goes first: " << std::endl;

    std::array<uint8_t, 2> scores = play_game(client_network, board_size);


    // Who won and score.
    std::cout << "Game over, scores: \n"
        << "Black Score: " << int(scores[0]) << std::endl
        << "White Score: " << int(scores[1]) << std::endl;
}
