// Copyright [2016] <duncan@wduncanfraser.com>
// Implementation of scalable_go_ab_prune

#include <algorithm>
#include <array>
#include <vector>
#include <cstdint>
#include <limits>
#include <cmath>

#include "ab_prune.h"
#include "gogame.h"

std::vector<uint8_t> get_board_segments(const uint8_t board_size) {
    // Validate appropriate board size was passed.
    if ((board_size < 3) || (board_size > 19)) {
        throw ABPruneSegmentError();
    }

    // Look at macros defining segments and create array of possible segments less than or equal to board size.
    std::vector<uint8_t> segments;

    for (uint8_t i = SEGMENT_MIN; i <= SEGMENT_MAX && i <= board_size; i+= SEGMENT_DIVISION) {
        segments.push_back(i);
    }

    // Validate board_size is the last element, or throw
    if (segments.back() == board_size) {
        return segments;
    } else {
        throw ABPruneSegmentError();
    }
}

std::vector<std::vector<double>> scalable_go_network_translation(const GoGame &i_gogame, const bool color) {
    // Get board size
    uint8_t board_size = i_gogame.get_size();
    // Get the board segments
    std::vector<uint8_t> board_segments = get_board_segments(board_size);
    // Get the board
    GoBoard goboard = i_gogame.get_board();

    // Create vector for holding segments.
    std::vector<std::vector<double>> output;

    // Setup the color flags
    uint8_t friendly_mask, enemy_mask;

    if (color) {
        friendly_mask = WHITE_MASK;
        enemy_mask = BLACK_MASK;
    } else {
        friendly_mask = BLACK_MASK;
        enemy_mask = WHITE_MASK;
    }

    // Start creating segments, starting with the smallest
    for (const auto &segment_size : board_segments) {
        // Loop through starting coordinates for segment size (always start at 0,0)
        for (uint8_t y_start = 0; y_start <= board_size - segment_size; y_start++) {
            for (uint8_t x_start = 0; x_start <= board_size - segment_size; x_start++) {
                // Setup vector to hold segment
                std::vector<double> temp_segment(segment_size * segment_size, 0);
                // Populate segment with specified starting coordinates
                for (uint8_t y = 0; y < segment_size; y++) {
                    for (uint8_t x = 0; x < segment_size; x++) {
                        // Temporary variable to hold segment value, default to 0 (no piece)
                        double segment_value = 0;
                        // Temporary variable to hold mask calculation.
                        uint8_t mask_value = goboard.board[y_start + y][x_start + x] & uint8_t(TEAM_MASK);

                        // If friendly, set value to 1. Else if enemy, set to -1.
                        if (mask_value == friendly_mask) {
                            segment_value = 1;
                        } else if (mask_value == enemy_mask) {
                            segment_value = -1;
                        }
                        // Assign segment_value to temp_segment
                        temp_segment[(y * segment_size) + x] = segment_value;
                    }
                }
                // Append segment to output
                output.push_back(temp_segment);
            }
        }
    }

    // All segments should now be created and appended to output.
    // Now, append remaining piece count and prisoner count to end of list.
    // TODO(wdfraser): Implement piece and prisoner count
    return output;
}

double scalable_go_ab_prune(GoBoardNeuralNet &network, GoGame &i_gogame, const int depth, double alpha, double beta,
                            const bool move_color, const bool max_player, const bool player_color) {
    // Generate moves and retrieve the move list
    i_gogame.generate_moves(move_color);
    std::vector<GoMove> current_move_list = i_gogame.get_move_list();

    // If this is the depth limit, or a leaf, calculate and return
    if ((depth <= 0) || (current_move_list.size() <=0 )) {
        std::vector<std::vector<double>> network_translation = scalable_go_network_translation(i_gogame, player_color);

        network.feed_forward(network_translation);
        return network.get_output();
    }

    if (max_player) {
        for (GoMove &element : current_move_list) {
            // Duplicate the existing board and make the move
            GoGame temp_board(i_gogame);
            temp_board.make_move(element, move_color);
            alpha = std::max(alpha, scalable_go_ab_prune(network, temp_board, depth-1, alpha, beta, !move_color,
                                                    false, player_color));
            if (beta <= alpha) {
                break;
            }
        }
        return alpha;
    } else {
        for (GoMove &element : current_move_list) {
            // Duplicate the existing board and make the move
            GoGame temp_board(i_gogame);
            temp_board.make_move(element, move_color);
            beta = std::min(beta, scalable_go_ab_prune(network, temp_board, depth-1, alpha, beta, !move_color,
                                                  true, player_color));
            if (beta <= alpha) {
                break;
            }
        }
        return beta;
    }
}

GoBoardNeuralNet::GoBoardNeuralNet(const uint8_t board_size) {
    // Get the segment sizes for the board_size. get_board_segments will throw if board is not of proper size
    std::vector<uint8_t> segments = get_board_segments(board_size);

    std::vector<uint16_t> segment_counts;

    for (uint8_t &segment : segments) {
        segment_counts.push_back((board_size - segment + uint16_t(1)) * (board_size - segment + uint16_t(1)));
    }

    for (uint16_t i = 0; i <= segments.size(); i++) {

    }
}

GoBoardNeuralNet::GoBoardNeuralNet(const GoBoardNeuralNet &i_network) { }

bool GoBoardNeuralNet::operator==(const GoBoardNeuralNet &i_network) const { }

GoBoardNeuralNet::~GoBoardNeuralNet() { }

int GoBoardNeuralNet::feed_forward(const std::vector<std::vector<double>> &input) {
    // TODO(wdfraser): Implement
    return 0;
}

const double GoBoardNeuralNet::get_output() const {
    // TODO(wdfraser): Implement
    return 0;
}
