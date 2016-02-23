// Implementation of Move Struct and GoBoard class
// Author: W. Duncan Fraser
// Email: duncan@wduncanfraser.com

#include <vector>
#include <cstdint>
#include <stdexcept>

#include "goboard.h"

Move::Move(const std::vector<std::vector<uint8_t>> &i_board, const uint8_t i_piece_x, const uint8_t i_piece_y) {
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

    // Board check complete, assign board
    board = i_board;

    // Check that move is within bounds
    if (! this->within_bounds(i_piece_x, i_piece_y)) {
        throw GoBoardInitError();
    }
    piece_x = i_piece_x;
    piece_y = i_piece_y;
}

bool Move::operator==(const Move &i_move) const {
    return (board == i_move.board) && (piece_x == i_move.piece_x) && (piece_y == i_move.piece_y);
}

const inline bool Move::within_bounds(const uint8_t x, const uint8_t y) const {
    // Check to make sure coordinates are not greater than board size. Both are unsigned, so no <0 checks required.
    return ((x < board.size()) && (y < board.size()));
}

int Move::check_move(const bool color) {
    // Check if the calculation has already been made (piece placed), as that is the most basic end case
    if (board[piece_y][piece_x] != 0) {
        return -1;
    }
    // Place the piece on the board
    board[piece_y][piece_x] = get_mask(color);

    // Default liberty to 0
    uint8_t liberty = 0;

    // Calculate the effects on the board
    // Check adjacent pieces to see if they are part of the same string, blank, or the enemy
    // Start Left
    if (this->within_bounds(piece_x - uint8_t(1), piece_y)) {
        if (board[piece_y][piece_x - uint8_t(1)] == 0) {
            liberty += 1;
        }
    }
    // Right
    if (this->within_bounds(piece_x + uint8_t(1), piece_y)) {
        if (board[piece_y][piece_x + uint8_t(1)] == 0) {
            liberty += 1;
        }
    }
    // Down
    if (this->within_bounds(piece_x, piece_y - uint8_t(1))) {
        if (board[piece_y - uint8_t(1)][piece_x] == 0) {
            liberty += 1;
        }
    }
    // UP
    if (this->within_bounds(piece_x, piece_y + uint8_t(1))) {
        if (board[piece_y + uint8_t(1)][piece_x] == 0) {
            liberty += 1;
        }
    }
    return int(liberty);
}

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

const std::vector<Move> GoBoard::get_move_list() const {
    return move_list;
}

const inline bool GoBoard::within_bounds(const uint8_t x, const uint8_t y) const {
    // Check to make sure coordinates are not greater than board size. Both are unsigned, so no <0 checks required.
    return ((x < board.size()) && (y < board.size()));
}

const bool GoBoard::check_move_history(const Move &i_move) const {
    for (const Move &row: move_history) {
        if (row.board == i_move.board) {
            return true;
        }
    }
    return false;
}

bool GoBoard::generate_moves(const bool color) {
    // Clear out anything already in the move list
    move_list.clear();

    // Get the board size
    uint8_t board_size = this->get_size();

    // Go through board element by element, checking for valid moves
    for (uint8_t y = 0; y < board_size; y++) {
        for (uint8_t x = 0; x < board_size; x++) {
            // Check if there is a piece first, as that is the most basic end case
            if (board[y][x] != 0) {
                continue;
            }
            // p short for potential. Create a move with the piece to be assigned
            Move p_board(board, x, y);

            //Based on the placed piece, determine the effect on the board
            p_board.check_move(color);

            //Check if the outcome is a preexisting board state
            if (this->check_move_history(p_board)) {
                continue;
            }

            // All checks have passed, append to the move_list
            move_list.push_back(p_board);
        }
    }

    // Return true if
    return move_list.size() != 0;
}

