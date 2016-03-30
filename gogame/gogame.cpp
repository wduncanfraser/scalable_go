// Copyright [2016] <duncan@wduncanfraser.com>
// Implementation of GoMove Struct and GoGame class

#include <algorithm>
#include <vector>
#include <cstdint>

#include "gogame.h"

XYCoordinate::XYCoordinate() : x(0), y(0) { }

XYCoordinate::XYCoordinate(const uint8_t i_x, const uint8_t i_y) : x(i_x), y(i_y) { }

bool XYCoordinate::operator==(const XYCoordinate &i_coordinate) const {
    return (x == i_coordinate.x) && (y == i_coordinate.y);
}

XYCoordinate& XYCoordinate::operator=(const XYCoordinate &i_coordinate) {
    if (this != &i_coordinate) {
        // Copy coordinates
        x = i_coordinate.x;
        y = i_coordinate.y;
    }
    return *this;
}

GoBoard::GoBoard(const uint8_t board_size) {
    // Check that board dimensions are between 3 and 19, otherwise throw
    if ((board_size < 3) || (board_size > 19)) {
        throw GoBoardInitError();
    }

    // Resize board vector to board dimensions. Initialize all cells to 0.
    board.resize(board_size);

    for (std::vector<uint8_t> &row : board) {
        row.resize(board_size, 0);
    }
}

GoBoard::GoBoard(const std::vector<std::vector<uint8_t>> &i_board) {
    // Check that board dimensions are between 3 and 19, and square; otherwise throw.
    uint64_t x_dim = i_board.size();

    // Checking X dimension
    if ((x_dim < 3) || (x_dim > 19)) {
        throw GoBoardInitError();
    }

    // Check each nested vector Y dimension
    for (const std::vector<uint8_t> &row : i_board) {
        if (row.size() != x_dim) {
            throw GoBoardInitError();
        }
    }

    // Checks complete, assign board
    board = i_board;
}

GoBoard::GoBoard(const GoBoard &i_goboard) : board(i_goboard.board) { }

bool GoBoard::operator==(const GoBoard &i_goboard) const {
    return (board == i_goboard.board);
}

GoBoard& GoBoard::operator=(const GoBoard &i_goboard) {
    if (this != &i_goboard) {
        // Copy variables
        board = i_goboard.board;
    }
    return *this;
}

const uint8_t GoBoard::get_size() const {
    // Validate we don't have an out of bound size. If we do, throw an Unknown Error.
    if ((board.size() < 3) || (board.size() > 19)) {
        throw GoBoardUnknownError();
    }
    return (uint8_t) board.size();
}

const inline bool GoBoard::within_bounds(const XYCoordinate &i_piece) const {
    // Check to make sure coordinates are not greater than board size. Both are unsigned, so no <0 checks required.
    return ((i_piece.x < board.size()) && (i_piece.y < board.size()));
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
    if (this != &i_string) {
        // Copy variables
        members = i_string.members;
        liberty = i_string.liberty;
        board_size = i_string.board_size;
    }
    return *this;
}

bool GoString::append_member(const XYCoordinate &coordinate) {
    // Ensure member is not in liberty
    if (liberty.size() > 0) {
        if (std::find(liberty.begin(), liberty.end(), coordinate) != liberty.end()) {
            return false;
        }
    }

    // Setup variable for tracking if adjacent
    bool adjacent = false;
    // Check if valid member
    // If this is the first element, nothing to check
    if (members.size() > 0) {
        // Check if coordinate is already in members
        if (std::find(members.begin(), members.end(), coordinate) != members.end()) {
            return false;
        }
        // Check if the coordinate is adjacent to existing members
        // Rather than checking every element in the string for adjacency, get the adjacent pieces around coordinate
        // and do a std::find against members
        std::vector<XYCoordinate> adjacent_coordinates = get_adjacent(coordinate, board_size);

        for (const XYCoordinate &element : adjacent_coordinates) {
            if (std::find(members.begin(), members.end(), element) != members.end()) {
                // If found, set adjacent to true and break
                adjacent = true;
                break;
            }
        }
    } else {
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
        if (std::find(members.begin(), members.end(), coordinate) != members.end()) {
            return false;
        }
    } else {
        return false;
    }

    // Setup variable for tracking if adjacent
    bool adjacent = false;

    // Check if valid liberty member
    // Check if coordinate is already in liberty
    if (std::find(liberty.begin(), liberty.end(), coordinate) != liberty.end()) {
        return false;
    }
    // Check if the coordinate is adjacent to existing members
    // Rather than checking every element in the string for adjacency, get the adjacent pieces around coordinate
    // and do a std::find against members
    std::vector<XYCoordinate> adjacent_coordinates = get_adjacent(coordinate, board_size);

    for (const XYCoordinate &element : adjacent_coordinates) {
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

GoMove::GoMove(const GoBoard &i_goboard, const XYCoordinate &i_piece) : goboard(i_goboard) {
    // Check that move is within bounds
    if (!goboard.within_bounds(i_piece)) {
        throw GoBoardInitError();
    }
    piece = i_piece;
}

bool GoMove::operator==(const GoMove &i_move) const {
    return (goboard == i_move.goboard) && (piece == i_move.piece);
}

GoMove& GoMove::operator=(const GoMove &i_move) {
    if (this != &i_move) {
        // Copy variables
        goboard = i_move.goboard;
        piece = i_move.piece;
    }
    return *this;
}

const GoString GoMove::construct_string(GoString i_string, const bool color) const {
    // take the passed string, and determine all the elements and liberty
    // Setup list of coordinates to check, starting with passed members
    std::vector<XYCoordinate> coordinates_check = i_string.get_members();

    while (coordinates_check.size() != 0) {
        // TODO(wdfraser): Make this faster
        // Check adjacency for the first element
        std::vector<XYCoordinate> adjacent_coordinates =
                get_adjacent(coordinates_check.front(), uint8_t(goboard.get_size()));
        for (XYCoordinate &element : adjacent_coordinates) {
            if (goboard.board[element.y][element.x] == 0) {
                // If blank, attempt to append to liberty. Fail silently if already part of liberty.
                i_string.append_liberty(element);
            } else if (goboard.board[element.y][element.x] == get_mask(color)) {
                // If part of string, attempt to append to members.
                // If successful appending to members, add to check list
                if (i_string.append_member(element)) {
                    coordinates_check.push_back(element);
                }
            }
        }

        // Done checking, remove first element from check list
        coordinates_check.erase(coordinates_check.begin());
    }

    return i_string;
}

bool GoMove::remove_string(const GoString &i_string, const bool color) {
    // Setup temporary board to test string against
    GoBoard temp_board(goboard);
    // Loop through each element, removing elements
    for (const XYCoordinate &element : i_string.get_members()) {
        if (temp_board.board[element.y][element.x] == get_mask(color)) {
            temp_board.board[element.y][element.x] = 0;
        } else {
            return false;
        }
    }

    // If we made it here, all elements were removed from temp_board correctly. set goboard to temp_board
    // and return true
    goboard = temp_board;
    return true;
}

int GoMove::check_move(const bool color) {
    // Check if the calculation has already been made (piece placed), as that is the most basic end case
    if (goboard.board[piece.y][piece.x] != 0) {
        return -1;
    }
    // Place the piece on the board
    goboard.board[piece.y][piece.x] = get_mask(color);

    // Calculate the effects on the board
    // Check adjacent pieces to see if they are part of the same string, blank, or the enemy
    GoString move_string(uint8_t(goboard.get_size()));
    // Append the placed piece to the String
    move_string.append_member(XYCoordinate(piece.x, piece.y));

    // Setup vector to hold enemy pieces
    std::vector<XYCoordinate> enemy_pieces;

    // Get the adjacent pieces and check their classification
    std::vector<XYCoordinate> adjacent_pieces =
            get_adjacent(XYCoordinate(piece.x, piece.y), uint8_t(goboard.get_size()));

    for (const XYCoordinate &element : adjacent_pieces) {
        if (goboard.board[element.y][element.x] == 0) {
            move_string.append_liberty(element);
        } else if (goboard.board[element.y][element.x] == get_mask(color)) {
            move_string.append_member(element);
        } else {
            enemy_pieces.push_back(element);
        }
    }

    // Check impact on enemy pieces
    for (const XYCoordinate &element : enemy_pieces) {
        // Double check that the piece still exists (wasn't remove as part of a prior string)
        // If empty, skip
        if (goboard.board[element.y][element.x] == 0) {
            continue;
        }
        GoString temp_string(uint8_t(goboard.get_size()));
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

    return static_cast<int>(move_string.get_liberty_count());
}

GoGame::GoGame(const uint8_t board_size) : goboard(board_size) {
    // Set flags
    move_list_dirty = true;
}

GoGame::GoGame(const GoBoard &i_goboard) : goboard(i_goboard) {
    // Set flags
    move_list_dirty = true;
}

GoGame::GoGame(const GoGame &i_gogame) : goboard(i_gogame.goboard), move_list(i_gogame.move_list),
                                             move_history(i_gogame.move_history),
                                             move_list_dirty(i_gogame.move_list_dirty),
                                             move_list_color(i_gogame.move_list_color) { }

bool GoGame::operator==(const GoGame &i_gogame) const {
    return (goboard == i_gogame.goboard) && (move_history == i_gogame.move_history);
}

const uint8_t GoGame::get_size() const {
    return goboard.get_size();
}

const GoBoard GoGame::get_board() const {
    return goboard;
}

void GoGame::set_board(const GoBoard &i_goboard) {
    // Validate provided board matches the board size
    if (i_goboard.get_size() != goboard.get_size()) {
        throw GoBoardInitError();
    }

    goboard = i_goboard;
}

const std::vector<GoMove> GoGame::get_move_list() const {
    return move_list;
}

const inline bool GoGame::within_bounds(const XYCoordinate &i_coord) const {
    return goboard.within_bounds(i_coord);
}

const bool GoGame::check_move_history(const GoMove &i_move) const {
    for (const GoMove &row : move_history) {
        if (row.goboard == i_move.goboard) {
            return true;
        }
    }
    return false;
}

bool GoGame::generate_moves(const bool color) {
    // Check if move list is dirty, so we don't generate the same list
    if ((move_list_dirty) || (move_list_color != color)) {
        // Clear out anything already in the move list
        move_list.clear();

        // Get the board size
        uint8_t board_size = this->get_size();

        // Go through board element by element, checking for valid moves
        for (uint8_t y = 0; y < board_size; y++) {
            for (uint8_t x = 0; x < board_size; x++) {
                // Check if there is a piece first, as that is the most basic end case
                if (goboard.board[y][x] != 0) {
                    continue;
                }
                // p short for potential. Create a move with the piece to be assigned
                GoMove p_board(goboard, XYCoordinate(x, y));

                // Based on the placed piece, determine the effect on the board
                // Check if the move is a suicide (has no liberty), as suicides are not allowed
                if (p_board.check_move(color) == 0) {
                    continue;
                }

                // Check if the outcome is a preexisting board state
                if (this->check_move_history(p_board)) {
                    continue;
                }

                // All checks have passed, append to the move_list
                move_list.push_back(p_board);
            }
        }

        // Set move_list flags
        move_list_dirty = false;
        move_list_color = color;
    }

    // Return true if there are any moves
    return move_list.size() != 0;
}

void GoGame::make_move(const GoMove &i_move) {
    // Not validating size as that is handled implicitly by comparing against the move list
    // Set bool for move color
    bool move_color;
    // Get the move color
    if (i_move.goboard.board[i_move.piece.y][i_move.piece.x] == get_mask(0)) {
        move_color = 0;
    } else if (i_move.goboard.board[i_move.piece.y][i_move.piece.x] == get_mask(1)) {
        move_color = 1;
    } else {
        throw GoBoardBadMove();
    }

    // Generate moves
    this->generate_moves(move_color);

    // Check if move is in move list
    if (std::find(move_list.begin(), move_list.end(), i_move) != move_list.end()) {
        // GoMove is valid, update board
        move_history.push_back(i_move);
        goboard = i_move.goboard;

        // Set move_list to dirty
        move_list_dirty = true;
    } else {
        // Not a valid move, throw
        throw GoBoardBadMove();
    }
}
