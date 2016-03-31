// Copyright [2016] <duncan@wduncanfraser.com>
// Test and visual inspection for basic movesets

#include <vector>
#include <iostream>
#include <cstdint>

#include "gogame.h"

void render_board(const GoBoard &i_goboard) {
    uint8_t board_size = i_goboard.get_size();

    for (int y = board_size - 1; y > -1; y--) {
        for (uint8_t x = 0; x < board_size; x++) {
            std::cout << "[" << int(i_goboard.board[y][x]) << "] ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    uint8_t board_size = 3;
    GoGame test(board_size);
    test.generate_moves(0);

    std::cout << "First move on 3x3 board: \n";
    for (const GoMove &element : test.get_move_list()) {
        std::cout << "Possible move: \n";

        render_board(element.get_board());
    }

    GoBoard test2_input(board_size);
    test2_input.board[0] = {get_mask(0), 0, get_mask(0)};
    test2_input.board[1] = {get_mask(1), get_mask(0), get_mask(1)};
    test2_input.board[2] = {0, get_mask(1), 0};
    GoGame test2(test2_input);
    test2.generate_moves(0);

    std::cout << "Black: No suicide on 3x3 board: \n";
    std::cout << "Initial state: \n";
    render_board(test2.get_board());

    for (const GoMove &element : test2.get_move_list()) {
        std::cout << "Possible move: \n";

        render_board(element.get_board());
    }

    test2.generate_moves(1);

    std::cout << "White: Capture on 3x3 board: \n";
    std::cout << "Initial state: \n";
    render_board(test2.get_board());

    for (const GoMove &element : test2.get_move_list()) {
        std::cout << "Possible move: \n";

        render_board(element.get_board());
    }

    test2.make_move(test2.get_move_list()[0]);
    std::cout << "White: Take move 1 on 3x3 board: \n";

    render_board(test2.get_board());


    test2.generate_moves(0);

    std::cout << "Black: No follow up possible \n";
    for (const GoMove &element : test2.get_move_list()) {
        std::cout << "Possible move: \n";

        render_board(element.get_board());
    }
}
