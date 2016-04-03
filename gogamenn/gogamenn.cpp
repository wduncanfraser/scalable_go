// Copyright [2016] <duncan@wduncanfraser.com>
// Implementation of scalable_go_ab_prune

#include <algorithm>
#include <array>
#include <vector>
#include <cstdint>
#include <limits>
#include <cmath>

#include "gogamenn.h"

std::vector<uint8_t> get_go_board_segments(const uint8_t board_size) {
    // Validate appropriate board size was passed.
    if ((board_size < 3) || (board_size > 19)) {
        throw GoGameNNSegmentError();
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
        throw GoGameNNSegmentError();
    }
}

std::vector<std::vector<double>> get_go_network_translation(const GoGame &i_gogame, const bool color) {
    // Get board size
    uint8_t board_size = i_gogame.get_size();
    // Get the board segments
    std::vector<uint8_t> board_segments = get_go_board_segments(board_size);
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
    return output;
}

GoGameNN::GoGameNN(const uint8_t i_board_size) {
    // Check that board dimensions are between 3 and 19, otherwise throw
    if ((i_board_size < 3) || (i_board_size > 19)) {
        throw GoGameNNInitError();
    }
    board_size = i_board_size;

    // Get the segment sizes for the board_size. get_go_board_segments will throw if board is not of proper size
    std::vector<uint8_t> segments = get_go_board_segments(board_size);

    for (uint8_t &segment : segments) {
        // Not using pow, to avoid converting to floating point
        uint16_t segment_count = (board_size - segment + uint16_t(1)) * (board_size - segment + uint16_t(1));

        // Determine Network dimensions
        std::vector<unsigned int> neuron_counts;
        neuron_counts.push_back(segment * segment);
        neuron_counts.push_back((neuron_counts[0] * 4) / 3);
        neuron_counts.push_back(neuron_counts[1] / 4);
        neuron_counts.push_back(1);

        // Creating a network for each segment.
        for (uint16_t i = 0; i < segment_count; i++) {
            layer1.push_back(NeuralNet(4, neuron_counts));
        }
    }

    // Create layer 2 network
    // Determine Network dimensions
    std::vector<unsigned int> neuron_counts;
    neuron_counts.push_back(layer1.size() + 3);
    neuron_counts.push_back((neuron_counts[0] * 2) / 3);
    neuron_counts.push_back(1);
    layer2 = NeuralNet(3, neuron_counts);
}

GoGameNN::GoGameNN(const GoGameNN &i_network) : board_size(i_network.board_size), layer1(i_network.layer1),
                                                layer2(i_network.layer2) { }

bool GoGameNN::operator==(const GoGameNN &i_network) const {
    return (layer1 == i_network.layer1) && (layer2 == i_network.layer2) && (board_size == i_network.board_size);
}

void GoGameNN::initialize_random() {
    // Randomize each network in Layer 1
    for (NeuralNet &element : layer1) {
        element.initialize_random();
    }

    // Initialize Layer 2
    layer2.initialize_random();
}

void GoGameNN::mutate(const double &radius) {
    // Mutate each network in Layer 1
    for (NeuralNet &element : layer1) {
        element.mutate(radius);
    }

    // Mutate layer 2
    layer2.mutate(radius);
}

void GoGameNN::feed_forward(const std::vector<std::vector<double>> &input_segments, const uint8_t pieces_played,
                                    const uint8_t prisoner_count, const uint8_t opponent_prisoner_count) {
    // Validate size of input_segments to layer 1 neural networks
    if (input_segments.size() != layer1.size()) {
        throw GoGameNNFeedForwardError();
    }

    // Vector to hold layer2 inputs.
    std::vector<double> layer2_inputs(layer1.size(), 0);

    // Pass all inputs to appropriate neural networks and feed forward
    for (unsigned int i = 0; i < layer1.size(); i++) {
        layer1[i].feed_forward(input_segments[i]);
        // Once complete. Assign output to layer 2 input.
        layer2_inputs[i] = layer1[i].get_output()[0];
    }

    // Layer 1 is processed. Append values for piece and prisoner counts to input.
    // TODO(wdfraser): Normalize these values
    layer2_inputs.push_back(pieces_played);
    layer2_inputs.push_back(prisoner_count);
    layer2_inputs.push_back(opponent_prisoner_count);

    // Feed forward Layer 2
    layer2.feed_forward(layer2_inputs);
}

const double GoGameNN::get_output() const {
    return layer2.get_output()[0];
}

void GoGameNN::export_weights_stream(std::ofstream &file) {
    // Export all layer 1 networks 1 by 1
    for (NeuralNet &element : layer1) {
        element.export_weights_stream(file);
    }
    // Export layer 2 network
    layer2.export_weights_stream(file);
}

void GoGameNN::import_weights_stream(std::ifstream &file) {
    // Import all layer 1 networks 1 by 1
    for (NeuralNet &element : layer1) {
        element.import_weights_stream(file);
    }
    // Import layer 2 network
    layer2.import_weights_stream(file);
}
