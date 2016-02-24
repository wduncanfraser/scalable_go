//
// Created by W. Duncan Fraser on 2/22/16.
//

#include <vector>
#include <iostream>
#include <cstdint>

#include "goboard.h"

int main() {
    uint8_t board_size = 3;
    GoBoard test(board_size);
    test.generate_moves(0);

    std::cout << "First move on 3x3 board: \n";
    for (const Move &element : test.get_move_list()) {
        std::cout << "Possible move: \n";
        for (int y = board_size - 1; y > -1; y--) {
            for (uint8_t x = 0; x < board_size; x++) {
                std::cout << "[" << int(element.board[y][x]) << "] ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    std::vector<std::vector<uint8_t>> test2_input;
    test2_input.resize(3);
    test2_input[0] = {get_mask(0), 0, get_mask(0)};
    test2_input[1] = {get_mask(1), get_mask(0), get_mask(1)};
    test2_input[2] = {0, get_mask(1), 0};
    GoBoard test2(test2_input);
    test2.generate_moves(0);

    std::cout << "Black: No suicide on 3x3 board: \n";
    for (const Move &element : test2.get_move_list()) {
        std::cout << "Possible move: \n";
        for (int y = board_size - 1; y > -1; y--) {
            for (uint8_t x = 0; x < board_size; x++) {
                std::cout << "[" << int(element.board[y][x]) << "] ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    test2.generate_moves(1);

    std::cout << "White: Capture on 3x3 board: \n";
    for (const Move &element : test2.get_move_list()) {
        std::cout << "Possible move: \n";
        for (int y = board_size - 1; y > -1; y--) {
            for (uint8_t x = 0; x < board_size; x++) {
                std::cout << "[" << int(element.board[y][x]) << "] ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    test2.make_move(test2.get_move_list()[0]);
    std::cout << "White: Take move 1 on 3x3 board: \n";
    for (int y = board_size - 1; y > -1; y--) {
        for (uint8_t x = 0; x < board_size; x++) {
            std::cout << "[" << int(test2.get_board()[y][x]) << "] ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;


    test2.generate_moves(0);

    std::cout << "Black: No follow up possible \n";
    for (const Move &element : test2.get_move_list()) {
        std::cout << "Possible move: \n";
        for (int y = board_size - 1; y > -1; y--) {
            for (uint8_t x = 0; x < board_size; x++) {
                std::cout << "[" << int(element.board[y][x]) << "] ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}
