// Copyright [2016] <duncan@wduncanfraser.com>
// Helper functions, such as rendering a gameboard for debugging purposes

#ifndef GOGAME_GOHELPERS_H_
#define GOGAME_GOHELPERS_H_

#include <iostream>

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

#endif  // GOGAME_GOHELPERS_H_
