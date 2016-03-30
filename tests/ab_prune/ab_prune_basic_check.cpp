// Copyright [2016] <duncan@wduncanfraser.com>

#include <vector>
#include <cstdint>
#include <iostream>
#include <string>
#include "gtest/gtest.h"

#include "ab_prune.h"

TEST(ab_prune_basic_check, manual_size_check) {
    // Validate result matches manually specified result
    std::vector<uint8_t> expected_segments { 3, 5, 7 };

    EXPECT_EQ(expected_segments, get_board_segments(7));
}

TEST(ab_prune_basic_check, all_segment_size_check) {
    // Validates that the right segment list is returned for each board size
    std::vector<std::vector<uint8_t>> expected_segments, obtained_segments;

    for (uint8_t i = SEGMENT_MIN; i <= SEGMENT_MAX; i+= SEGMENT_DIVISION) {
        std::vector<uint8_t> temp_segments;
        for (uint8_t j = SEGMENT_MIN; j <= i; j += SEGMENT_DIVISION) {
            temp_segments.push_back(j);
        }
        expected_segments.push_back(temp_segments);
        obtained_segments.push_back(get_board_segments(i));
    }
    EXPECT_EQ(expected_segments, obtained_segments);
}

TEST(ab_prune_basic_check, improper_segment_size) {
    // Validate an improper segment size throws error
    EXPECT_THROW(get_board_segments(4), ABPruneSegmentError);
}

TEST(ab_prune_basic_check, basic_blank_translation) {
    // Validate basic translation for a 5x5 blank board is correct.
    GoGame test_board(5);
    std::vector<std::vector<double>> expected_translation;

    // Assign expected values
    for (uint8_t i = 0; i < 9; i++) {
        expected_translation.push_back(std::vector<double> { 0, 0, 0, 0, 0, 0, 0, 0, 0 });
    }

    std::vector<double> temp_vector;

    for (uint8_t i = 0; i < 25; i++) {
        temp_vector.push_back(0);
    }

    expected_translation.push_back(temp_vector);

    EXPECT_EQ(expected_translation, scalable_go_network_translation(test_board, true));
}

TEST(ab_prune_basic_check, populated_3x3_board_translation_black) {
    GoBoard test_board({{get_mask(false), 0, get_mask(false)}, {get_mask(true), get_mask(false), get_mask(true)},
                        {0, get_mask(true), 0}});
    GoGame test_game(test_board);

    std::vector<std::vector<double>> expected_translation {{ 1, 0, 1, -1, 1, -1, 0, -1, 0 }};

    EXPECT_EQ(expected_translation, scalable_go_network_translation(test_game, 0));
}

TEST(ab_prune_basic_check, populated_3x3_board_translation_white) {
    GoBoard test_board({{get_mask(false), 0, get_mask(false)}, {get_mask(true), get_mask(false), get_mask(true)},
                        {0, get_mask(true), 0}});
    GoGame test_game(test_board);

    std::vector<std::vector<double>> expected_translation {{ -1, 0, -1, 1, -1, 1, 0, 1, 0 }};

    EXPECT_EQ(expected_translation, scalable_go_network_translation(test_game, 1));
}
