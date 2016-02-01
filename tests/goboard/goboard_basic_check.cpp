//
// Created by W. Duncan Fraser on 1/28/16.
//

#include <vector>
#include <array>
#include <cstdint>
#include "gtest/gtest.h"

#include "goboard.h"

TEST(goboard_basic_check, board_initialization_size) {
    // Validate that a board initialized with a specific size is of that size
    std::array<uint8_t, 17> board_inputs{ {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19} };
    std::array<uint8_t, 17> board_outputs;

    for(uint8_t i = 0; i < 17; i++) {
        GoBoard test_board(board_inputs[i]);
        board_outputs[i] = test_board.get_size();
    }
    EXPECT_EQ(board_inputs, board_outputs);
}

TEST(goboard_basic_check, board_initialization_out_of_range_small) {
    // Validate that GoBoard throws init error if board is too small
    EXPECT_THROW(GoBoard test_board(2), GoBoardInitError);
}

TEST(goboard_basic_check, board_initialization_out_of_range_large) {
    // Validate that GoBoard throws init error if board is too small
    EXPECT_THROW(GoBoard test_board(20), GoBoardInitError);
}

TEST(goboard_basic_check, board_initialized_blank) {
    // Validate that a board initialized with a specific size is initialized blank and of the correct size
    std::array<uint8_t, 17> board_inputs{ {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19} };
    std::array<std::vector<std::vector<uint8_t>>, 17> expected_boards;
    std::array<std::vector<std::vector<uint8_t>>, 17> output_boards;

    for(uint8_t i = 0; i < 17; i++) {
        // Initialize expected board
        expected_boards[i].resize(board_inputs[i]);
        for (std::vector<uint8_t> &row: expected_boards[i]) {
            row.assign(board_inputs[i], 0);
        }

        // Create real board
        GoBoard test_board(board_inputs[i]);

        // Get board output
        output_boards[i] = test_board.get_board();
    }
    EXPECT_EQ(expected_boards, output_boards);
}

TEST(goboard_basic_check, board_copy_blank) {
    // Validate that a board intialized with copy constructor from a blank board is equal.
    GoBoard test1(5);
    GoBoard test2(test1);
    EXPECT_EQ(test1, test2);
}
