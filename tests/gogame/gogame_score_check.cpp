// Copyright [2016] <duncan@wduncanfraser.com>

#include <vector>
#include <array>
#include <cstdint>
#include "gtest/gtest.h"

#include "gogame.h"


TEST(gogame_score_check, blank_board_black) {
    uint8_t board_size = 3;
    GoGame test(board_size);

    std::array<uint8_t, 2> expected { {0, 0} };

    EXPECT_EQ(expected, test.calculate_scores(get_mask(0)));
}

TEST(gogame_score_check, blank_board_white) {
    uint8_t board_size = 3;
    GoGame test(board_size);

    std::array<uint8_t, 2> expected { {0, 0} };

    EXPECT_EQ(expected, test.calculate_scores(get_mask(1)));
}

TEST(gogame_score_check, single_piece_black) {
    uint8_t board_size = 3;
    GoBoard test_board(board_size);

    test_board.board[1][1] = get_mask(0);

    GoGame test(test_board);

    std::array<uint8_t, 2> expected { {8, 0} };

    EXPECT_EQ(expected, test.calculate_scores(0));
}

TEST(gogame_score_check, single_piece_black_white_score) {
    uint8_t board_size = 3;
    GoBoard test_board(board_size);

    test_board.board[1][1] = get_mask(0);

    GoGame test(test_board);

    std::array<uint8_t, 2> expected { {0, 8} };

    EXPECT_EQ(expected, test.calculate_scores(1));
}

TEST(gogame_score_check, single_piece_white) {
    uint8_t board_size = 3;
    GoBoard test_board(board_size);

    test_board.board[1][1] = get_mask(1);

    GoGame test(test_board);

    std::array<uint8_t, 2> expected { {8, 0} };

    EXPECT_EQ(expected, test.calculate_scores(1));
}

TEST(gogame_score_check, single_piece_white_black_score) {
    uint8_t board_size = 3;
    GoBoard test_board(board_size);

    test_board.board[1][1] = get_mask(1);

    GoGame test(test_board);

    std::array<uint8_t, 2> expected { {0, 8} };

    EXPECT_EQ(expected, test.calculate_scores(0));
}

TEST(gogame_score_check, simple_split_board_black) {
    uint8_t board_size = 3;
    GoBoard test_board(board_size);

    test_board.board[0] = {get_mask(1), get_mask(1), 0};
    test_board.board[1] = {get_mask(0), get_mask(0), get_mask(0)};

    GoGame test(test_board);

    std::array<uint8_t, 2> expected { {3, 0} };

    EXPECT_EQ(expected, test.calculate_scores(0));
}

TEST(gogame_score_check, simple_split_board_white) {
    uint8_t board_size = 3;
    GoBoard test_board(board_size);

    test_board.board[0] = {get_mask(1), get_mask(1), 0};
    test_board.board[1] = {get_mask(0), get_mask(0), get_mask(0)};

    GoGame test(test_board);

    std::array<uint8_t, 2> expected { {0, 3} };

    EXPECT_EQ(expected, test.calculate_scores(1));
}

TEST(gogame_score_check, simple_game_1) {
    std::vector<XYCoordinate> move_list { {0, 1}, {0, 0}, {1, 1}, {1, 0}, {2, 1}, {0, 2}, {2, 0}, {1, 2}, {2, 2} };
    bool current_color = 0;

    uint8_t board_size = 3;
    GoGame test(board_size);

    for (const XYCoordinate &element : move_list) {
        GoMove temp_move(test.get_board(), element);
        temp_move.check_move(current_color);
        test.make_move(temp_move);
        current_color = !current_color;
    }

    std::array<uint8_t, 2> expected { {8, 0} };
    EXPECT_EQ(expected, test.calculate_scores(0));
}
