// Prototypes for Move Struct and GoBoard Class
// Author: W. Duncan Fraser
// Email: duncan@wduncanfraser.com

#ifndef SCALABLE_GO_GOBOARD_H
#define SCALABLE_GO_GOBOARD_H

#define BLACK_MASK 1
#define WHITE_MASK 3
#define STONE_COUNT 180

// GoBoard exceptions
class GoBoardInitError : public std::runtime_error {
public:
    GoBoardInitError() : std::runtime_error("GoBoardInitError") { }
};

class GoBoardUnknownError : public std::runtime_error {
public:
    GoBoardUnknownError() : std::runtime_error("GoBoardUnknownError") { }
};

// Helper functions
// Function to get piece mask in int format from bool
// black = 0, white = 1
inline uint8_t get_mask(const bool color) {
    if (color) {
        return uint8_t(WHITE_MASK);
    }
    else {
        return uint8_t(BLACK_MASK);
    }
}

// Struct for holding a possible move
struct Move {
    // Resultant Game Board
    std::vector<std::vector<uint8_t>> board;
    // Piece placed
    uint8_t piece_x, piece_y;

    // Constructor
    Move(const std::vector<std::vector<uint8_t>> &i_board,
         const uint8_t i_piece_x, const uint8_t i_piece_y);

    // Comparison Operator
    bool operator==(const Move &i_move) const;

    // Check to make sure coordinates are within the bounds of the Go Board.
    // Returns true if within bounds. Otherwise, false.
    const inline bool within_bounds(const uint8_t x, const uint8_t y) const;

    // Determines the impact of a move on the board. Returns -1 if the piece has already been placed, otherwise
    // returns the liberty of the piece/string
    // black = 0, white = 1
    int check_move(const bool color);
};

class GoBoard {
private:
    // Game board
    std::vector<std::vector<uint8_t>> board;
    // Move list. Cached list of possible moves from current board state.
    std::vector<Move> move_list;
    // Move history
    std::vector<Move> move_history;
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

    //Function to get the move list
    const std::vector<Move> get_move_list() const;

    // Check to make sure coordinates are within the bounds of the Go Board.
    // Returns true if within bounds. Otherwise, false.
    const inline bool within_bounds(const uint8_t x, const uint8_t y) const;

    // Checks if moves has been made before
    // Returns true if board state has previously existed.
    const bool check_move_history(const Move &i_move) const;

    // Generate all possible moves for specified color.
    // black = 0, white = 1
    // Return value of 0 signifies no moves.
    bool generate_moves(const bool color);

    // Makes a move
    bool make_move(const Move &i_move);
};


#endif //SCALABLE_GO_GOBOARD_H
