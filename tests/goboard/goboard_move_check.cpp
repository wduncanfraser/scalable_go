//
// Created by W. Duncan Fraser on 2/22/16.
//

#include <vector>
#include <array>
#include <cstdint>
#include "gtest/gtest.h"

#include "goboard.h"


TEST(goboard_move_check, move_already_calculated) {
    uint8_t board_size = 3;
    GoBoard test(board_size);

    Move test_move(test.get_board(), 0, 0);
    test_move.check_move(0);

    EXPECT_EQ(-1, test_move.check_move(0));
}

TEST(goboard_move_check, single_piece_full_liberty) {
    uint8_t board_size = 3;
    GoBoard test(board_size);

    Move test_move(test.get_board(), 1, 1);

    EXPECT_EQ(4, test_move.check_move(0));
}

TEST(goboard_move_check, single_piece_full_liberty_side) {
    uint8_t board_size = 3;
    GoBoard test(board_size);

    Move test_move(test.get_board(), 1, 0);

    EXPECT_EQ(3, test_move.check_move(0));
}

TEST(goboard_move_check, single_piece_full_liberty_corner) {
    uint8_t board_size = 3;
    GoBoard test(board_size);

    Move test_move(test.get_board(), 0, 0);

    EXPECT_EQ(2, test_move.check_move(0));
}

TEST(goboard_move_check, single_piece_surrounded) {
    uint8_t board_size = 3;
    GoBoard test(board_size);

    Move test_move(test.get_board(), 1, 1);
    test_move.board[0][1] = get_mask(1);
    test_move.board[2][1] = get_mask(1);
    test_move.board[1][0] = get_mask(1);
    test_move.board[1][2] = get_mask(1);

    EXPECT_EQ(0, test_move.check_move(0));
}
