// Implementation of Move Struct and GoBoard class
// Author: W. Duncan Fraser
// Email: duncan@wduncanfraser.com

#include <vector>
#include <algorithm>
#include <cstdint>
#include <stdexcept>

#include "goboard.h"

XYCoordinate::XYCoordinate() : x(0), y(0) { }

XYCoordinate::XYCoordinate(const uint8_t i_x, const uint8_t i_y) : x(i_x), y(i_y) { }

bool XYCoordinate::operator==(const XYCoordinate &i_coordinate) const {
    return (x == i_coordinate.x) && (y == i_coordinate.y);
}

XYCoordinate& XYCoordinate::operator=(const XYCoordinate &i_coordinate) {
    if(this != &i_coordinate)
    {
        //Copy coordinates
        x = i_coordinate.x;
        y = i_coordinate.y;
    }
    return *this;
}

GoString::GoString(uint8_t i_board_size) {
    // Validate max is less than maximum board size, or throw
    if (i_board_size > 19) {
        throw GoBoardInitError();
    }
    board_size = i_board_size;
}

GoString::~GoString() { }

bool GoString::operator==(const GoString &i_string) const {
    return (members == i_string.members) && (liberty == i_string.liberty) && (board_size == i_string.board_size);
}

GoString& GoString::operator=(const GoString &i_string) {
    if(this != &i_string)
    {
        //Copy variables
        members = i_string.members;
        liberty = i_string.liberty;
        board_size = i_string.board_size;
    }
    return *this;
}

bool GoString::append_member(const XYCoordinate &coordinate) {
    // Ensure member is not in liberty
    if (liberty.size() > 0) {
        if (std::find(liberty.begin(), liberty.end(), coordinate) != liberty.end())
        {
            return false;
        }
    }

    // Setup variable for tracking if adjacent
    bool adjacent = false;
    // Check if valid member
    // If this is the first element, nothing to check
    if (members.size() > 0) {
        // Check if coordinate is already in members
        if (std::find(members.begin(), members.end(), coordinate) != members.end())
        {
            return false;
        }
        // Check if the coordinate is adjacent to existing members
        // Rather than checking every element in the string for adjacency, get the adjacent pieces around coordinate
        // and do a std::find against members
        std::vector<XYCoordinate> adjacent_coordinates = get_adjacent(coordinate, board_size);

        for (const XYCoordinate &element: adjacent_coordinates) {
            if (std::find(members.begin(), members.end(), element) != members.end()) {
                // If found, set adjacent to true and break
                adjacent = true;
                break;
            }
        }
    }
    else {
        // override adjacent to true for the first member
        adjacent = true;
    }

    // If we got this far, and piece is adjacent, append
    if (adjacent) {
        members.push_back(coordinate);
    }
    return adjacent;
}

bool GoString::append_liberty(const XYCoordinate &coordinate) {
    // Ensure liberty coordinate is not in members, and that members has at least 1 element
    if (members.size() > 0) {
        if (std::find(members.begin(), members.end(), coordinate) != members.end())
        {
            return false;
        }
    }
    else {
        return false;
    }

    // Setup variable for tracking if adjacent
    bool adjacent = false;

    // Check if valid liberty member
    // Check if coordinate is already in liberty
    if (std::find(liberty.begin(), liberty.end(), coordinate) != liberty.end())
    {
        return false;
    }
    // Check if the coordinate is adjacent to existing members
    // Rather than checking every element in the string for adjacency, get the adjacent pieces around coordinate
    // and do a std::find against members
    std::vector<XYCoordinate> adjacent_coordinates = get_adjacent(coordinate, board_size);

    for (const XYCoordinate &element: adjacent_coordinates) {
        if (std::find(members.begin(), members.end(), element) != members.end()) {
            // If found, set adjacent to true and break
            adjacent = true;
            break;
        }
    }

    // If we got this far, and piece is adjacent, append
    if (adjacent) {
        liberty.push_back(coordinate);
    }
    return adjacent;
}

const std::vector<XYCoordinate> GoString::get_members() const {
    return members;
}

const std::vector<XYCoordinate> GoString::get_liberty() const {
    return liberty;
}

const uint8_t GoString::get_member_count() const {
    return uint8_t(members.size());
}

const uint8_t GoString::get_liberty_count() const {
    return uint8_t(liberty.size());
}

const uint8_t GoString::get_size() const {
    return board_size;
}

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

Move& Move::operator=(const Move &i_move) {
    if(this != &i_move)
    {
        //Copy variables
        board = i_move.board;
        piece_x = i_move.piece_x;
        piece_y = i_move.piece_y;
    }
    return *this;
}

const inline bool Move::within_bounds(const uint8_t x, const uint8_t y) const {
    // Check to make sure coordinates are not greater than board size. Both are unsigned, so no <0 checks required.
    return ((x < board.size()) && (y < board.size()));
}

const GoString Move::construct_string(GoString i_string, const bool color) const {
    // take the passed string, and determine all the elements and liberty
    // Setup list of coordinates to check, starting with passed members
    std::vector<XYCoordinate> coordinates_check = i_string.get_members();

    while(coordinates_check.size() != 0) {
        // TODO: Make this faster
        // Check adjacency for the first element
        std::vector<XYCoordinate> adjacent_coordinates = get_adjacent(coordinates_check.front(), uint8_t(board.size()));
        for (XYCoordinate &element: adjacent_coordinates) {
            if (board[element.y][element.x] == 0) {
                // If blank, attempt to append to liberty. Fail silently if already part of liberty.
                i_string.append_liberty(element);
            }
            else if (board[element.y][element.x] == get_mask(color)) {
                // If part of string, attempt to append to members. If successful appending to members, add to check list
                if(i_string.append_member(element)) {
                    coordinates_check.push_back(element);
                }
            }
        }

        // Done checking, remove first element from check list
        coordinates_check.erase(coordinates_check.begin());
    }

    return i_string;
}

bool Move::remove_string(const GoString &i_string, const bool color) {
    // Setup temporary board to test string against
    std::vector<std::vector<uint8_t>> temp_board = board;
    // Loop through each element, removing elements
    for (const XYCoordinate &element: i_string.get_members()) {
        if (temp_board[element.y][element.x] == get_mask(color)) {
            temp_board[element.y][element.x] = 0;
        }
        else
        {
            return false;
        }
    }

    // If we made it here, all elements were removed from temp_board correctly. set board to temp_board
    // and return true
    board = temp_board;
    return true;
}

int Move::check_move(const bool color) {
    // Check if the calculation has already been made (piece placed), as that is the most basic end case
    if (board[piece_y][piece_x] != 0) {
        return -1;
    }
    // Place the piece on the board
    board[piece_y][piece_x] = get_mask(color);

    // Calculate the effects on the board
    // Check adjacent pieces to see if they are part of the same string, blank, or the enemy
    GoString move_string(uint8_t(board.size()));
    // Append the placed piece to the String
    move_string.append_member(XYCoordinate(piece_x, piece_y));

    // Setup vector to hold enemy pieces
    std::vector<XYCoordinate> enemy_pieces;

    // Get the adjacent pieces and check their classification
    std::vector<XYCoordinate> adjacent_pieces = get_adjacent(XYCoordinate(piece_x, piece_y), uint8_t(board.size()));

    for (const XYCoordinate &element: adjacent_pieces) {
        if (board[element.y][element.x] == 0) {
            move_string.append_liberty(element);
        }
        else if (board[element.y][element.x] == get_mask(color)) {
            move_string.append_member(element);
        }
        else {
            enemy_pieces.push_back(element);
        }
    }

    // Check impact on enemy pieces
    for (const XYCoordinate &element: enemy_pieces) {
        // Double check that the piece still exists (wasn't remove as part of a prior string)
        // If empty, skip
        if (board[element.y][element.x] == 0) {
            continue;
        }
        GoString temp_string(uint8_t(board.size()));
        temp_string.append_member(element);
        temp_string = this->construct_string(temp_string, !color);

        // If the enemy string has no liberty, remove from the board
        if (temp_string.get_liberty_count() == 0) {
            if (!this->remove_string(temp_string, !color)) {
                // If we failed to remove the string we just built... something is very very wrong
                throw GoBoardUnknownError();
            }
        }
    }

    // Construct the string to determine string liberty
    // On single pieces, This will account for new liberty due to removed pieces
    move_string = this->construct_string(move_string, color);

    // If the move_string has 0 liberty, remove it from the board
    if (move_string.get_liberty_count() == 0) {
        if (!this->remove_string(move_string, color)) {
            // If we failed to remove the string we just built... something is very very wrong
            throw GoBoardUnknownError();
        }
    }

    return int(move_string.get_liberty_count());
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

    // Return true if there are any moves
    return move_list.size() != 0;
}

