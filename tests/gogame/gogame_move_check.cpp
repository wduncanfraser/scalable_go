// Copyright [2016] <duncan@wduncanfraser.com>

#include <vector>
#include <array>
#include <cstdint>
#include "gtest/gtest.h"

#include "gogame.h"


TEST(gogame_move_check, move_already_calculated) {
    uint8_t board_size = 3;
    GoGame test(board_size);

    GoMove test_move(test.get_board(), XYCoordinate(0, 0));
    test_move.check_move(0);

    EXPECT_EQ(-1, test_move.check_move(0));
}

TEST(gogame_move_check, single_piece_full_liberty) {
    uint8_t board_size = 3;
    GoGame test(board_size);

    GoMove test_move(test.get_board(), XYCoordinate(1, 1));

    EXPECT_EQ(4, test_move.check_move(0));
}

TEST(gogame_move_check, single_piece_full_liberty_side) {
    uint8_t board_size = 3;
    GoGame test(board_size);

    GoMove test_move(test.get_board(), XYCoordinate(1, 0));

    EXPECT_EQ(3, test_move.check_move(0));
}

TEST(gogame_move_check, single_piece_full_liberty_corner) {
    uint8_t board_size = 3;
    GoGame test(board_size);

    GoMove test_move(test.get_board(), XYCoordinate(0, 0));

    EXPECT_EQ(2, test_move.check_move(0));
}

TEST(gogame_move_check, single_piece_surrounded) {
    uint8_t board_size = 3;
    GoBoard test_board(board_size);
    test_board.board[0][1] = get_mask(1);
    test_board.board[2][1] = get_mask(1);
    test_board.board[1][0] = get_mask(1);
    test_board.board[1][2] = get_mask(1);

    GoMove test_move(test_board, XYCoordinate(1, 1));

    EXPECT_EQ(0, test_move.check_move(0));
}

TEST(gogame_move_check, blank_board_passes) {
    uint8_t board_size = 3;
    GoGame test(board_size);

    GoMove pass_move(test.get_board());

    test.make_move(pass_move, 0);
    test.make_move(pass_move, 1);

    GoBoard expected_board(board_size);

    EXPECT_EQ(expected_board, test.get_board());
}

TEST(gogame_move_check, repeate_moves_history) {
    uint8_t board_size = 5;
    GoBoard test_board(board_size);

    test_board.board[1] = {0, get_mask(0), 0, get_mask(0), 0};
    test_board.board[2] = {0, get_mask(1), get_mask(0), get_mask(1), 0};
    test_board.board[3][2] = get_mask(1);

    GoGame test_game(test_board);

    // Make first move
    GoMove test_move1(test_game.get_board(), XYCoordinate(0, 2));
    test_move1.check_move(0);
    test_game.make_move(test_move1, 0);

    // Make capture
    GoMove test_move2(test_game.get_board(), XYCoordinate(2, 1));
    test_move2.check_move(1);
    test_game.make_move(test_move2, 1);

    // Attempt to make second capture which would reset board to a prior state
    GoMove test_move3(test_game.get_board(), XYCoordinate(2, 2));
    test_move3.check_move(0);

    // Check move is in move history
    EXPECT_EQ(true, test_game.check_move_history(test_move3));
}

TEST(gogame_move_check, repeate_moves_throw) {
    uint8_t board_size = 5;
    GoBoard test_board(board_size);

    test_board.board[1] = {0, get_mask(0), 0, get_mask(0), 0};
    test_board.board[2] = {0, get_mask(1), get_mask(0), get_mask(1), 0};
    test_board.board[3][2] = get_mask(1);

    GoGame test_game(test_board);

    // Make first move
    GoMove test_move1(test_game.get_board(), XYCoordinate(0, 2));
    test_move1.check_move(0);
    test_game.make_move(test_move1, 0);

    // Make capture
    GoMove test_move2(test_game.get_board(), XYCoordinate(2, 1));
    test_move2.check_move(1);
    test_game.make_move(test_move2, 1);

    // Attempt to make second capture which would reset board to a prior state
    GoMove test_move3(test_game.get_board(), XYCoordinate(2, 2));
    test_move3.check_move(0);

    // Check move is in move history
    EXPECT_THROW(test_game.make_move(test_move3, 0), GoBoardBadMove);
}

TEST(gogame_move_check, simple_game_1) {
    std::vector<XYCoordinate> move_list { {0, 1}, {0, 0}, {1 ,1}, {1, 0}, {2, 1}, {0, 2}, {2, 0}, {1, 2}, {2, 2} };
    bool current_color = 0;

    uint8_t board_size = 3;
    GoGame test(board_size);

    for (const XYCoordinate &element : move_list) {
        GoMove temp_move(test.get_board(), element);
        temp_move.check_move(current_color);
        test.make_move(temp_move, current_color);
        current_color = !current_color;
    }

    GoBoard expected(board_size);
    expected.board = { {0, 0, get_mask(0)},
                       {get_mask(0), get_mask(0), get_mask(0)},
                       {0, 0, get_mask(0)}};

    EXPECT_EQ(expected, test.get_board());
}

// TODO(wdfraser): Need a lot more unit testing on move generation
