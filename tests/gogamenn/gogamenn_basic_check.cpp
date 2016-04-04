// Copyright [2016] <duncan@wduncanfraser.com>

#include <vector>
#include <cstdint>
#include <iostream>
#include <string>
#include "gtest/gtest.h"

#include "gogame.h"
#include "gogamenn.h"

TEST(gogamenn_basic_check, manual_size_check) {
    // Validate result matches manually specified result
    std::vector<uint8_t> expected_segments { 3, 5, 7, 9 };

    EXPECT_EQ(expected_segments, get_go_board_segments(9));
}

TEST(gogamenn_basic_check, all_segment_size_check) {
    // Validates that the right segment list is returned for each board size
    std::vector<std::vector<uint8_t>> expected_segments, obtained_segments;

    for (uint8_t i = SEGMENT_MIN; i <= SEGMENT_MAX; i+= SEGMENT_DIVISION) {
        std::vector<uint8_t> temp_segments;
        for (uint8_t j = SEGMENT_MIN; j <= i; j += SEGMENT_DIVISION) {
            temp_segments.push_back(j);
        }
        expected_segments.push_back(temp_segments);
        obtained_segments.push_back(get_go_board_segments(i));
    }
    EXPECT_EQ(expected_segments, obtained_segments);
}

TEST(gogamenn_basic_check, improper_segment_size) {
    // Validate an improper segment size throws error
    EXPECT_THROW(get_go_board_segments(4), GoGameNNSegmentError);
}

TEST(gogamenn_basic_check, basic_blank_translation) {
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

    EXPECT_EQ(expected_translation, get_go_network_translation(test_board, true));
}

TEST(gogamenn_basic_check, populated_3x3_board_translation_black) {
    GoBoard test_board({{get_mask(false), 0, get_mask(false)}, {get_mask(true), get_mask(false), get_mask(true)},
                        {0, get_mask(true), 0}});
    GoGame test_game(test_board);

    std::vector<std::vector<double>> expected_translation {{ 1, 0, 1, -1, 1, -1, 0, -1, 0 }};

    EXPECT_EQ(expected_translation, get_go_network_translation(test_game, 0));
}

TEST(gogamenn_basic_check, populated_3x3_board_translation_white) {
    GoBoard test_board({{get_mask(false), 0, get_mask(false)}, {get_mask(true), get_mask(false), get_mask(true)},
                        {0, get_mask(true), 0}});
    GoGame test_game(test_board);

    std::vector<std::vector<double>> expected_translation {{ -1, 0, -1, 1, -1, 1, 0, 1, 0 }};

    EXPECT_EQ(expected_translation, get_go_network_translation(test_game, 1));
}

TEST(gogamenn_basic_check, gogamenn_5x5_blank) {
    uint8_t board_size = 5;
    GoGameNN test_nn(board_size, false);

    GoGame test_game(board_size);
    test_nn.feed_forward(get_go_network_translation(test_game, 0), test_game.get_pieces_placed()[0],
                         test_game.get_prisoner_count()[0], test_game.get_prisoner_count()[1]);

    EXPECT_EQ(0, test_nn.get_output());

    test_nn.feed_forward(get_go_network_translation(test_game, 1), test_game.get_pieces_placed()[1],
                         test_game.get_prisoner_count()[1], test_game.get_prisoner_count()[0]);

    EXPECT_EQ(0, test_nn.get_output());
}

TEST(gogamenn_basic_check, gogamenn_too_small) {
    EXPECT_THROW(GoGameNN(2, false), GoGameNNInitError);
}

TEST(gogamenn_basic_check, gogamenn_too_large) {
    EXPECT_THROW(GoGameNN(20, false), GoGameNNInitError);
}

TEST(gogamenn_basic_check, gogamenn_copy_compare) {
    uint8_t board_size = 5;
    GoGameNN test_nn(board_size, false);

    test_nn.initialize_random();

    GoGameNN test_nn2(test_nn);

    EXPECT_EQ(test_nn, test_nn2);
}

TEST(gogamenn_basic_check, gogamenn_5x5_randomized) {
    uint8_t board_size = 5;
    GoGameNN test_nn(board_size, false);
    test_nn.initialize_random();

    GoGame test_game(board_size);
    test_nn.feed_forward(get_go_network_translation(test_game, 0), test_game.get_pieces_placed()[0],
                         test_game.get_prisoner_count()[0], test_game.get_prisoner_count()[1]);

    EXPECT_NE(0, test_nn.get_output());
}

TEST(gogamenn_basic_check, gogamenn_5x5_blank_opponent_check) {
    uint8_t board_size = 5;
    GoGameNN test_nn(board_size, false);
    test_nn.initialize_random();

    GoGame test_game(board_size);
    test_nn.feed_forward(get_go_network_translation(test_game, 0), test_game.get_pieces_placed()[0],
                         test_game.get_prisoner_count()[0], test_game.get_prisoner_count()[1]);
    double team1 = test_nn.get_output();

    test_nn.feed_forward(get_go_network_translation(test_game, 1), test_game.get_pieces_placed()[1],
                         test_game.get_prisoner_count()[1], test_game.get_prisoner_count()[0]);
    double team2 = test_nn.get_output();

    EXPECT_EQ(team1, team2);
}

TEST(gogamenn_basic_check, gogamenn_5x5_single_piece_opponent_check) {
    uint8_t board_size = 5;
    GoBoard test_board(board_size);
    test_board.board[0][0] = get_mask(0);
    test_board.board[2][2] = get_mask(1);
    GoGame test_game(test_board);

    GoGameNN test_nn(board_size, false);
    test_nn.initialize_random();

    test_nn.feed_forward(get_go_network_translation(test_game, 0), test_game.get_pieces_placed()[0],
                         test_game.get_prisoner_count()[0], test_game.get_prisoner_count()[1]);
    double team1 = test_nn.get_output();

    test_nn.feed_forward(get_go_network_translation(test_game, 1), test_game.get_pieces_placed()[1],
                         test_game.get_prisoner_count()[1], test_game.get_prisoner_count()[0]);
    double team2 = test_nn.get_output();

    EXPECT_NE(team1, team2);
}

TEST(gogamenn_basic_check, gogamenn_5x5_mutate) {
    uint8_t board_size = 5;
    GoGameNN test_nn(board_size, false);
    test_nn.initialize_random();

    GoGame test_game(board_size);
    test_nn.feed_forward(get_go_network_translation(test_game, 0), test_game.get_pieces_placed()[0],
                         test_game.get_prisoner_count()[0], test_game.get_prisoner_count()[1]);

    double original_output = test_nn.get_output();

    test_nn.mutate(.01);

    test_nn.feed_forward(get_go_network_translation(test_game, 0), test_game.get_pieces_placed()[0],
                         test_game.get_prisoner_count()[0], test_game.get_prisoner_count()[1]);

    EXPECT_NE(original_output, test_nn.get_output());
}

TEST(gogamenn_basic_check, gogamenn_bad_feed_forward) {
    uint8_t board_size = 5;
    GoGameNN test_nn(board_size, false);
    test_nn.initialize_random();

    GoGame test_game(board_size);
    std::vector<std::vector<double>> test_feed = get_go_network_translation(test_game, 0);
    test_feed.erase(test_feed.end() - 1);

    EXPECT_THROW(test_nn.feed_forward(test_feed, test_game.get_pieces_placed()[0], test_game.get_prisoner_count()[0],
                                      test_game.get_prisoner_count()[1]), GoGameNNFeedForwardError);
}
TEST(gogamenn_basic_check, write_to_file) {
    // Check that there is no data loss/roundng error in exporting the network to file
    GoGameNN test_network1(5, false);
    test_network1.initialize_random();

    std::ofstream output_file("testgogamenn.txt");
    test_network1.export_weights_stream(output_file);
    output_file.close();

    GoGameNN test_network2(5, false);
    std::ifstream input_file("testgogamenn.txt");
    test_network2.import_weights_stream(input_file);
    input_file.close();

    EXPECT_EQ(test_network1, test_network2);
}

TEST(gogamenn_basic_check, write_multiple_to_file) {
    // Check that there is no data loss/roundng error in exporting networks to file
    std::vector<GoGameNN> test_networks1(10, GoGameNN(5, false));
    for (GoGameNN &element : test_networks1) {
        element.initialize_random();

    }

    std::ofstream output_file("testgogamenns.txt");
    for (GoGameNN &element : test_networks1) {
        element.export_weights_stream(output_file);
    }
    output_file.close();

    std::vector<GoGameNN> test_networks2(10, GoGameNN(5, false));

    std::ifstream input_file("testgogamenns.txt");
    for (GoGameNN &element : test_networks2) {
        element.import_weights_stream(input_file);
    }
    input_file.close();

    EXPECT_EQ(test_networks1, test_networks2);
}
