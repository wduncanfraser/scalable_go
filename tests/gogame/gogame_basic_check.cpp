// Copyright [2016] <duncan@wduncanfraser.com>

#include <vector>
#include <array>
#include <cstdint>
#include "gtest/gtest.h"

#include "gogame.h"

TEST(gogame_basic_check, mask_black_check) {
    // Validates that the right mask is given for black
    EXPECT_EQ(1, get_mask(0));
}

TEST(gogame_basic_check, mask_white_check) {
    // Validates that the right mask is given for white
    EXPECT_EQ(3, get_mask(1));
}

TEST(gogame_basic_check, board_initialization_size) {
    // Validate that a board initialized with a specific size is of that size
    std::array<uint8_t, 17> board_inputs{ {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19} };
    std::array<uint8_t, 17> board_outputs;

    for (uint8_t i = 0; i < 17; i++) {
        GoBoard test_board(board_inputs[i]);
        board_outputs[i] = test_board.get_size();
    }
    EXPECT_EQ(board_inputs, board_outputs);
}

TEST(gogame_basic_check, board_initialization_out_of_range_small) {
    // Validate that GoGame throws init error if board is too small
    EXPECT_THROW(GoBoard test_board(2), GoBoardInitError);
}

TEST(gogame_basic_check, board_initialization_out_of_range_large) {
    // Validate that GoGame throws init error if board is too small
    EXPECT_THROW(GoBoard test_board(20), GoBoardInitError);
}

TEST(gogame_basic_check, board_initialized_blank) {
    // Validate that a board initialized with a specific size is initialized blank and of the correct size
    std::array<uint8_t, 17> board_inputs{ {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19} };
    std::array<std::vector<std::vector<uint8_t>>, 17> expected_boards;
    std::array<std::vector<std::vector<uint8_t>>, 17> output_boards;

    for (uint8_t i = 0; i < 17; i++) {
        // Initialize expected board
        expected_boards[i].resize(board_inputs[i]);
        for (std::vector<uint8_t> &row : expected_boards[i]) {
            row.assign(board_inputs[i], 0);
        }

        // Create real board
        GoBoard test_board(board_inputs[i]);

        // Get board output
        output_boards[i] = test_board.board;
    }
    EXPECT_EQ(expected_boards, output_boards);
}

TEST(gogame_basic_check, board_copy_blank) {
    // Validate that a board intialized with copy constructor from a blank board is equal.
    GoBoard test1(5);
    GoBoard test2(test1);
    EXPECT_EQ(test1, test2);
}

TEST(gogame_basic_check, game_copy_blank) {
    // Validate that a board intialized with copy constructor from a blank board is equal.
    GoGame test1(5);
    GoGame test2(test1);
    EXPECT_EQ(test1, test2);
}

TEST(gogame_basic_check, first_move_generation_count) {
    GoGame test(5);
    test.generate_moves(0);
    EXPECT_EQ(static_cast<unsigned int>(26), test.get_move_list().size());
}

TEST(gogame_basic_check, first_move_generation) {
    uint8_t board_size = 3;
    GoGame test(board_size);
    test.generate_moves(0);

    std::vector<GoMove> test_move_list;
    for (uint8_t y = 0; y < board_size; y++) {
        for (uint8_t x = 0; x < board_size; x++) {
            // Setup a blank board
            GoBoard temp_board(board_size);

            // Assign piece to coordinates
            temp_board.board[y][x] = get_mask(0);
            test_move_list.push_back(GoMove(temp_board, XYCoordinate(x, y)));
        }
    }
    // Add pass
    test_move_list.push_back(GoMove(test.get_board()));

    EXPECT_EQ(test_move_list, test.get_move_list());
}
