// Copyright [2016] <duncan@wduncanfraser.com>

#include <vector>
#include <cstdint>
#include <iostream>
#include <string>
#include <limits>
#include "gtest/gtest.h"

#include "gogame.h"
#include "gogamenn.h"
#include "gogameab.h"

TEST(gogameab_basic_check, simple_ab) {
    uint8_t board_size = 5;
    int depth = 1;
    GoGame test_game(board_size);

    GoGameNN test_network(board_size, false);

    test_game.generate_moves(0);

    double best_move_value, temp_best_move_value = 0;
    GoMove best_move(test_game.get_board());

    best_move_value = -std::numeric_limits<double>::infinity();

    // For each possible move, calculate Alpha Beta
    for (const GoMove &element : test_game.get_move_list()) {
        GoGame temp_game(test_game);
        temp_game.make_move(element, 0);

        temp_best_move_value = scalable_go_ab_prune(test_network, temp_game, depth,
                                                    -std::numeric_limits<double>::infinity(),
                                                    std::numeric_limits<double>::infinity(), 1, false, 0);

        if (temp_best_move_value > best_move_value) {
            best_move_value = temp_best_move_value;
            best_move = element;
        }
    }

    EXPECT_NO_THROW(test_game.make_move(best_move, 0));
}

TEST(gogameab_basic_check, simple_ab_uniform) {
    uint8_t board_size = 5;
    int depth = 1;
    GoGame test_game(board_size);

    GoGameNN test_network(board_size, true);

    test_game.generate_moves(0);

    double best_move_value, temp_best_move_value = 0;
    GoMove best_move(test_game.get_board());

    best_move_value = -std::numeric_limits<double>::infinity();

    // For each possible move, calculate Alpha Beta
    for (const GoMove &element : test_game.get_move_list()) {
        GoGame temp_game(test_game);
        temp_game.make_move(element, 0);

        temp_best_move_value = scalable_go_ab_prune(test_network, temp_game, depth,
                                                    -std::numeric_limits<double>::infinity(),
                                                    std::numeric_limits<double>::infinity(), 1, false, 0);

        if (temp_best_move_value > best_move_value) {
            best_move_value = temp_best_move_value;
            best_move = element;
        }
    }

    EXPECT_NO_THROW(test_game.make_move(best_move, 0));
}


