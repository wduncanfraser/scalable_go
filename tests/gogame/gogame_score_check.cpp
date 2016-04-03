// Copyright [2016] <duncan@wduncanfraser.com>

#include <vector>
#include <array>
#include <cstdint>
#include "gtest/gtest.h"

#include "gogame.h"
#include "gohelpers.h"


TEST(gogame_score_check, blank_board) {
    uint8_t board_size = 3;
    GoGame test(board_size);

    std::array<uint8_t, 2> expected { {0, 0} };

    EXPECT_EQ(expected, test.calculate_scores());
}

TEST(gogame_score_check, single_piece_black) {
    uint8_t board_size = 3;
    GoBoard test_board(board_size);

    test_board.board[1][1] = get_mask(0);

    GoGame test(test_board);

    std::array<uint8_t, 2> expected { {8, 0} };

    EXPECT_EQ(expected, test.calculate_scores());
}

TEST(gogame_score_check, single_piece_white) {
    uint8_t board_size = 3;
    GoBoard test_board(board_size);

    test_board.board[1][1] = get_mask(1);

    GoGame test(test_board);

    std::array<uint8_t, 2> expected { {0, 8} };

    EXPECT_EQ(expected, test.calculate_scores());
}

TEST(gogame_score_check, blank_board_pass_black) {
    uint8_t board_size = 3;
    GoGame test(board_size);

    GoMove pass_move(test.get_board());

    test.make_move(pass_move, 0);

    std::array<uint8_t, 2> expected { {0, 1} };
    std::array<uint8_t, 2> expected_pieces { {1, 0} };

    EXPECT_EQ(expected, test.calculate_scores());
    EXPECT_EQ(expected_pieces, test.get_pieces_placed());
}

TEST(gogame_score_check, blank_board_pass_white) {
    uint8_t board_size = 3;
    GoGame test(board_size);

    GoMove pass_move(test.get_board());

    test.make_move(pass_move, 1);

    std::array<uint8_t, 2> expected { {1, 0} };
    std::array<uint8_t, 2> expected_pieces { {0, 1} };

    EXPECT_EQ(expected, test.calculate_scores());
    EXPECT_EQ(expected_pieces, test.get_pieces_placed());
}

TEST(gogame_score_check, blank_board_passes) {
    uint8_t board_size = 3;
    GoGame test(board_size);

    GoMove pass_move(test.get_board());

    test.make_move(pass_move, 0);
    test.make_move(pass_move, 1);

    std::array<uint8_t, 2> expected { {1, 1} };

    EXPECT_EQ(expected, test.calculate_scores());
    EXPECT_EQ(expected, test.get_pieces_placed());
}

TEST(gogame_score_check, simple_split_board) {
    uint8_t board_size = 3;
    GoBoard test_board(board_size);

    test_board.board[0] = {get_mask(1), get_mask(1), 0};
    test_board.board[1] = {get_mask(0), get_mask(0), get_mask(0)};

    GoGame test(test_board);

    std::array<uint8_t, 2> expected { {3, 0} };

    EXPECT_EQ(expected, test.calculate_scores());
}

TEST(gogame_score_check, simple_game_1) {
    std::vector<XYCoordinate> move_list { {0, 1}, {0, 0}, {1, 1}, {1, 0}, {2, 1}, {0, 2}, {2, 0}, {1, 2}, {2, 2} };
    bool current_color = 0;

    uint8_t board_size = 3;
    GoGame test(board_size);

    for (const XYCoordinate &element : move_list) {
        GoMove temp_move(test.get_board(), element);
        temp_move.check_move(current_color);
        test.make_move(temp_move, current_color);
        current_color = !current_color;
    }

    std::cout << std::endl;
    render_board(test.get_board());
    std::cout << "Black Prisoners: " << int(test.get_prisoner_count()[0]) <<
            " White Prisoners: " << int(test.get_prisoner_count()[1]) << std::endl;
    std::cout << "Black Pieces Placed: " << int(test.get_pieces_placed()[0]) <<
            " White Pieces Placed: " << int(test.get_pieces_placed()[1]) << std::endl;

    std::array<uint8_t, 2> expected { {8, 0} };
    EXPECT_EQ(expected, test.calculate_scores());
}

TEST(gogame_score_check, simple_game_2_52_moves) {
    // Example game from http://www.britgo.org/intro/intro3.html
    std::vector<XYCoordinate> move_list { {4, 4}, {2, 4}, {6, 4}, {3, 6}, {2, 3}, {1, 3}, {2, 2},
                                          {3, 4}, {4, 3}, {1, 2}, {4, 6}, {4, 7}, {5, 6}, {3, 3}, {3, 2}, {4, 2},
                                          {4, 1}, {5, 2}, {5, 1}, {6, 2}, {1, 1},
                                          {7, 4}, {7, 5}, {6, 3}, {5, 4}, {0, 1},
                                          {6, 1}, {7, 1}, {7, 0}, {7, 2}, {8, 3}, {2, 1}, {3, 1}, {1, 0}, {7, 3}, {3, 0}, {5, 3}, {4, 0}, {8, 1},
                                          {3, 7}, {5, 7}, {5, 8}, {6, 8}, {4, 8}, {6, 7},
                                          {4, 5}, {5, 5}, {3, 5}, {8, 2}, {5, 0},
                                          {6, 0}, {2, 0} };
    bool current_color = 0;

    uint8_t board_size = 9;
    GoGame test(board_size);

    for (const XYCoordinate &element : move_list) {
        GoMove temp_move(test.get_board(), element);
        temp_move.check_move(current_color);
        test.make_move(temp_move, current_color);
        current_color = !current_color;
    }

    std::cout << std::endl;
    render_board(test.get_board());
    std::cout << "Black Prisoners: " << int(test.get_prisoner_count()[0]) <<
            " White Prisoners: " << int(test.get_prisoner_count()[1]) << std::endl;
    std::cout << "Black Pieces Placed: " << int(test.get_pieces_placed()[0]) <<
            " White Pieces Placed: " << int(test.get_pieces_placed()[1]) << std::endl;

    std::array<uint8_t, 2> expected { {13, 20} };
    std::array<uint8_t, 2> expected_pieces { {26, 26} };
    EXPECT_EQ(expected, test.calculate_scores());
    EXPECT_EQ(expected_pieces, test.get_pieces_placed());
}

TEST(gogame_score_check, simple_game_2_54_moves) {
    // Example game from http://www.britgo.org/intro/intro3.html
    std::vector<XYCoordinate> move_list { {4, 4}, {2, 4}, {6, 4}, {3, 6}, {2, 3}, {1, 3}, {2, 2},
                                          {3, 4}, {4, 3}, {1, 2}, {4, 6}, {4, 7}, {5, 6}, {3, 3}, {3, 2}, {4, 2},
                                          {4, 1}, {5, 2}, {5, 1}, {6, 2}, {1, 1},
                                          {7, 4}, {7, 5}, {6, 3}, {5, 4}, {0, 1},
                                          {6, 1}, {7, 1}, {7, 0}, {7, 2}, {8, 3}, {2, 1}, {3, 1}, {1, 0}, {7, 3}, {3, 0}, {5, 3}, {4, 0}, {8, 1},
                                          {3, 7}, {5, 7}, {5, 8}, {6, 8}, {4, 8}, {6, 7},
                                          {4, 5}, {5, 5}, {3, 5}, {8, 2}, {5, 0},
                                          {6, 0}, {2, 0},
                                          {8, 4}, {0, 0} };
    bool current_color = 0;

    uint8_t board_size = 9;
    GoGame test(board_size);

    for (const XYCoordinate &element : move_list) {
        GoMove temp_move(test.get_board(), element);
        temp_move.check_move(current_color);
        test.make_move(temp_move, current_color);
        current_color = !current_color;
    }

    std::cout << std::endl;
    render_board(test.get_board());
    std::cout << "Black Prisoners: " << int(test.get_prisoner_count()[0]) <<
            " White Prisoners: " << int(test.get_prisoner_count()[1]) << std::endl;

    std::cout << "Black Pieces Placed: " << int(test.get_pieces_placed()[0]) <<
            " White Pieces Placed: " << int(test.get_pieces_placed()[1]) << std::endl;

    std::array<uint8_t, 2> expected { {24, 19} };
    std::array<uint8_t, 2> expected_pieces { {27, 27} };
    EXPECT_EQ(expected, test.calculate_scores());
    EXPECT_EQ(expected_pieces, test.get_pieces_placed());
}

TEST(gogame_score_check, simple_game_2_54_moves_pass_ended) {
    // Example game from http://www.britgo.org/intro/intro3.html
    std::vector<XYCoordinate> move_list { {4, 4}, {2, 4}, {6, 4}, {3, 6}, {2, 3}, {1, 3}, {2, 2},
                                          {3, 4}, {4, 3}, {1, 2}, {4, 6}, {4, 7}, {5, 6}, {3, 3}, {3, 2}, {4, 2},
                                          {4, 1}, {5, 2}, {5, 1}, {6, 2}, {1, 1},
                                          {7, 4}, {7, 5}, {6, 3}, {5, 4}, {0, 1},
                                          {6, 1}, {7, 1}, {7, 0}, {7, 2}, {8, 3}, {2, 1}, {3, 1}, {1, 0}, {7, 3}, {3, 0}, {5, 3}, {4, 0}, {8, 1},
                                          {3, 7}, {5, 7}, {5, 8}, {6, 8}, {4, 8}, {6, 7},
                                          {4, 5}, {5, 5}, {3, 5}, {8, 2}, {5, 0},
                                          {6, 0}, {2, 0},
                                          {8, 4}, {0, 0} };
    bool current_color = 0;

    uint8_t board_size = 9;
    GoGame test(board_size);

    for (const XYCoordinate &element : move_list) {
        GoMove temp_move(test.get_board(), element);
        temp_move.check_move(current_color);
        test.make_move(temp_move, current_color);
        current_color = !current_color;
    }

    // Passes to end the game
    GoMove pass_move(test.get_board());
    // Black Pass
    test.make_move(pass_move, 0);
    // White Pass
    test.make_move(pass_move, 1);

    std::cout << std::endl;
    render_board(test.get_board());
    std::cout << "Black Prisoners: " << int(test.get_prisoner_count()[0]) <<
    " White Prisoners: " << int(test.get_prisoner_count()[1]) << std::endl;

    std::cout << "Black Pieces Placed: " << int(test.get_pieces_placed()[0]) <<
    " White Pieces Placed: " << int(test.get_pieces_placed()[1]) << std::endl;

    std::array<uint8_t, 2> expected { {25, 20} };
    std::array<uint8_t, 2> expected_pieces { {28, 28} };
    EXPECT_EQ(expected, test.calculate_scores());
    EXPECT_EQ(expected_pieces, test.get_pieces_placed());
}

TEST(gogame_score_check, simple_game_3) {
    // Example game from http://www.britgo.org/intro/intro3.html
    std::vector<XYCoordinate> move_list { {6, 5}, {3, 6}, {5, 2}, {2, 2},
                                          {4, 4}, {2, 4}, {4, 6}, {3, 7}, {4, 7},
                                          {4, 1}, {5, 1}, {4, 2}, {4, 3}, {3, 5}, {4, 5},
                                          {4, 8}, {5, 8}, {3, 8}, {5, 7},
                                          {3, 3}, {4, 0}, {3, 0}, {5, 0}, {3, 1}};
    bool current_color = 0;

    uint8_t board_size = 9;
    GoGame test(board_size);

    for (const XYCoordinate &element : move_list) {
        GoMove temp_move(test.get_board(), element);
        temp_move.check_move(current_color);
        test.make_move(temp_move, current_color);
        current_color = !current_color;
    }

    std::cout << std::endl;
    render_board(test.get_board());
    std::cout << "Black Prisoners: " << int(test.get_prisoner_count()[0]) <<
    " White Prisoners: " << int(test.get_prisoner_count()[1]) << std::endl;

    std::cout << "Black Pieces Placed: " << int(test.get_pieces_placed()[0]) <<
    " White Pieces Placed: " << int(test.get_pieces_placed()[1]) << std::endl;

    std::array<uint8_t, 2> expected { {30, 26} };
    std::array<uint8_t, 2> expected_pieces { {12, 12} };
    EXPECT_EQ(expected, test.calculate_scores());
    EXPECT_EQ(expected_pieces, test.get_pieces_placed());
}
