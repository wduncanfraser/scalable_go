// Implementation of Move Struct and GoBoard class
// Author: W. Duncan Fraser
// Email: duncan@wduncanfraser.com

#include <vector>
#include <cstdint>
#include <stdexcept>

#include "goboard.h"

GoBoard::GoBoard(const uint8_t board_size) {
    // Check that board dimensions are between 3 and 19, otherwise throw
    if ((board_size < 3) || (board_size > 19)) {
        throw GoBoardInitError();
    }

    // Resize board vector to board dimensions. Initialize all cells to 0.
    board.resize(board_size);

    for (std::vector<uint8_t> &row: board) {
        row.resize(board_size, 0);
    }
}

GoBoard::GoBoard(const std::vector<std::vector<uint8_t>> &i_board) {
    // Check that board dimensions are between 3 and 19, and square; otherwise throw.
    unsigned long x_dim = i_board.size();

    // Checking X dimension
    if ((x_dim < 3) || (x_dim > 19)) {
        throw GoBoardInitError();
    }

    // Check each nested vector Y dimension
    for (const std::vector<uint8_t> &row: i_board) {
        if (row.size() != x_dim) {
            throw GoBoardInitError();
        }
    }

    // Checks complete, assign board
    board = i_board;
}

GoBoard::GoBoard(const GoBoard &i_goboard) : board(i_goboard.board), move_list(i_goboard.move_list) { }

GoBoard::~GoBoard() { }

bool GoBoard::operator==(const GoBoard &i_board) const {
    return board == i_board.board;
}

const uint8_t GoBoard::get_size() const {
    // Validate we don't have an out of bound size. If we do, throw an Unknown Error.
    if ((board.size() < 3) || (board.size() > 19)) {
        throw GoBoardUnknownError();
    }
    return (uint8_t) board.size();
}

const std::vector<std::vector<uint8_t>> GoBoard::get_board() const {
    return board;
}

const inline bool GoBoard::within_bounds(const uint8_t x, const uint8_t y) const {
    // Check to make sure coordinates are not greater than board size. Both are unsigned, so no <0 checks required.
    return ((x < board.size()) && (y < board.size()));
}
