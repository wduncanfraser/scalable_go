// Prototypes for Move Struct and GoBoard Class
// Author: W. Duncan Fraser
// Email: duncan@wduncanfraser.com

#ifndef SCALABLE_GO_GOBOARD_H
#define SCALABLE_GO_GOBOARD_H

// GoBoard exceptions
class GoBoardInitError : public std::runtime_error {
public:
    GoBoardInitError() : std::runtime_error("GoBoardInitError") { }
};

class GoBoardUnknownError : public std::runtime_error {
public:
    GoBoardUnknownError() : std::runtime_error("GoBoardUnknownError") { }
};

// Struct for holding a possible move
struct Move {
    // Resultant Game Board
    std::vector<std::vector<uint8_t>> board;
    // Piece placed
    uint8_t piece_x, piece_y;

    // Constructor
    Move(const std::vector<std::vector<uint8_t>> &i_board,
         const uint8_t i_piece_x, const uint8_t i_piece_y);
};

class GoBoard {
private:
    // Game board
    std::vector<std::vector<uint8_t>> board;
    // Move list. Cached list of possible moves from current board state.
    std::vector<Move> move_list;
public:
    // Constructor with size specification
    GoBoard(const uint8_t board_size);

    // Constructor with board passed as a vector
    GoBoard(const std::vector<std::vector<uint8_t>> &i_board);

    // Copy Constructor
    GoBoard(const GoBoard &i_goboard);

    // Comparison Operator
    bool operator==(const GoBoard &i_board) const;

    // Destructor
    virtual ~GoBoard();

    // Function to get the board size
    const uint8_t get_size() const;

    // Function to get current board state
    const std::vector<std::vector<uint8_t>> get_board() const;

    // Check to make sure coordinates are within the bounds of the Go Board.
    // Returns true if within bounds. Otherwise, false.
    const inline bool within_bounds(const uint8_t x, const uint8_t y) const;
};


#endif //SCALABLE_GO_GOBOARD_H
