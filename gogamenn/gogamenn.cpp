// Copyright [2016] <duncan@wduncanfraser.com>
// Implementation of scalable_go_ab_prune

#include <algorithm>
#include <array>
#include <vector>
#include <cstdint>
#include <limits>
#include <cmath>
#include <random>

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

GoGameNN::GoGameNN(const uint8_t i_board_size, const bool i_uniform) {
    // Check that board dimensions are between 3 and 19, otherwise throw
    if ((i_board_size < 3) || (i_board_size > 19)) {
        throw GoGameNNInitError();
    }
    board_size = i_board_size;
    uniform = i_uniform;

    // Get the segment sizes for the board_size. get_go_board_segments will throw if board is not of proper size
    std::vector<uint8_t> segments = get_go_board_segments(board_size);

    // Store the count of layer 1 segments.
    uint16_t layer_1_segments = 0;

    for (uint8_t &segment : segments) {
        // Determine Network dimensions
        std::vector<unsigned int> neuron_counts;
        neuron_counts.push_back(segment * segment);
        neuron_counts.push_back((neuron_counts[0] * 4) / 3);
        neuron_counts.push_back(neuron_counts[1] / 4);
        neuron_counts.push_back(1);

        // Not using pow, to avoid converting to floating point
        uint16_t segment_count = (board_size - segment + uint16_t(1)) * (board_size - segment + uint16_t(1));
        layer_1_segments += segment_count;

        if (uniform) {
            // If uniform, we only need 1 network for each segment size.
            layer1.push_back(NeuralNet(4, neuron_counts));
        } else {
            // If not uniform, we need a network for each subsection
            // Creating a network for each segment.
            for (uint16_t i = 0; i < segment_count; i++) {
                layer1.push_back(NeuralNet(4, neuron_counts));
            }
        }
    }

    // Create layer 2 network
    // Determine Network dimensions
    std::vector<unsigned int> neuron_counts;
    neuron_counts.push_back(layer_1_segments + 3);
    neuron_counts.push_back((neuron_counts[0] * 2) / 3);
    neuron_counts.push_back(1);
    layer2 = NeuralNet(3, neuron_counts);
}

GoGameNN::GoGameNN(const GoGameNN &i_network) : uniform(i_network.uniform), board_size(i_network.board_size),
                                                layer1(i_network.layer1), layer2(i_network.layer2) { }

bool GoGameNN::operator==(const GoGameNN &i_network) const {
    return (layer1 == i_network.layer1) && (layer2 == i_network.layer2) && (board_size == i_network.board_size) &&
            (uniform == i_network.uniform);
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
    // Vector to hold layer2 inputs.
    std::vector<double> layer2_inputs(input_segments.size(), 0);

    if (uniform) {
        // Validate size of input_segments
        std::vector<uint8_t> segments = get_go_board_segments(board_size);
        std::vector<uint16_t> segment_counts;
        uint16_t segment_count = 0;

        for (uint8_t &segment : segments) {
            segment_counts.push_back((board_size - segment + uint16_t(1)) * (board_size - segment + uint16_t(1)));
            segment_count += segment_counts[segment_counts.size() - 1];
        }

        if (input_segments.size() != segment_count) {
            throw GoGameNNFeedForwardError();
        }

        unsigned int i = 0;
        for (uint16_t j = 0; j < segment_counts.size(); j++) {
            for (uint16_t k = 0; k < segment_counts[j]; k++) {
                layer1[j].feed_forward(input_segments[i]);
                // Once complete. Assign output to layer 2 input.
                layer2_inputs[i] = layer1[j].get_output()[0];
                i += 1;
            }
        }
    } else {
        // Validate size of input_segments to layer 1 neural networks
        if (input_segments.size() != layer1.size()) {
            throw GoGameNNFeedForwardError();
        }

        // Pass all inputs to appropriate neural networks and feed forward
        for (unsigned int i = 0; i < layer1.size(); i++) {
            layer1[i].feed_forward(input_segments[i]);
            // Once complete. Assign output to layer 2 input.
            layer2_inputs[i] = layer1[i].get_output()[0];
        }
    }
    // Layer 1 is processed. Append values for piece and prisoner counts to input.
    // Values are normalized to 1/2 the total board pieces rounded down. So for a 3x3 game. 9 pieces. Normalized by 4.
    uint8_t normalization = uint8_t((board_size * board_size) / 2);
    layer2_inputs.push_back(pieces_played / normalization);
    layer2_inputs.push_back(prisoner_count / normalization);
    layer2_inputs.push_back(opponent_prisoner_count / normalization);

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

void GoGameNN::scale_network(const GoGameNN &i_network) {
    // First, validate that we are 1 size larger than the passed network.
    if (board_size != (i_network.board_size + SEGMENT_DIVISION)) {
        throw GoGameNNScaleError();
    }

    if (uniform) {
        // Validate we are both uniform if the network is uniform.
        if (!i_network.uniform) {
            throw GoGameNNScaleError();
        }
        // Copy existing networks. Only 1 per segment size.
        for (u_int8_t i = 0; i < i_network.layer1.size(); i++) {
            layer1[i] = i_network.layer1[i];
        }
    } else {
        // Validate we are both not uniform if the network is not uniform.
        if (i_network.uniform) {
            throw GoGameNNScaleError();
        }
        // Set Random generator for use when selecting networks from previous generation
        std::random_device rd;
        std::mt19937 gen(rd());

        // Copy existing networks
        // We should end up with (board_size - segment_size + 1) ^ 2 of the previous size networks.
        // For example:
        // A 5x5 network would have 9 3x3 subsections
        // A 7x7 Network would have 9 5x5 subsections and 25 3x3 subsections.
        // A 9x9 Network would have 9 7x7 subsections, 25 5x5 subsections, and 49 3x3 subsections.
        // Setup int for storing current segment.
        unsigned int current_offset = 0;
        unsigned int previous_offset = 0;
        // Loop starting at the smallest subsection.
        for (uint8_t segment_size = SEGMENT_MIN; segment_size < board_size; segment_size += SEGMENT_DIVISION) {
            unsigned int total_segments = (board_size - segment_size + 1) * (board_size - segment_size + 1);
            unsigned int previous_total_segments = (board_size - segment_size - 1) * (board_size - segment_size - 1);

            // Loop through all segments of a given size. Randomly assign them an appropriate network from
            // the passed network.
            for (unsigned int segment = 0; segment < total_segments; segment++) {
                std::uniform_int_distribution<> dis(0, previous_total_segments - 1);

                layer1[current_offset + segment] = i_network.layer1[previous_offset + dis(gen)];
            }

            // Finished assigning current segment size. Increment offsets.
            current_offset += total_segments;
            previous_offset += previous_total_segments;
        }
    }

    // At this point, all scaling should be done. Need to randomly initialize the largest subsection and layer 2.
    layer1[layer1.size() - 1].initialize_random();
    layer2.initialize_random();
}

std::vector<NeuralNet> GoGameNN::get_layer1() {
    return layer1;
}
