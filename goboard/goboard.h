// Prototypes for Move Struct and GoBoard Class
// Author: W. Duncan Fraser
// Email: duncan@wduncanfraser.com

#ifndef SCALABLE_GO_GOBOARD_H
#define SCALABLE_GO_GOBOARD_H

#include <algorithm>

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

class GoBoardBadMove : public std::runtime_error {
public:
    GoBoardBadMove() : std::runtime_error("GoBoardBadMove") { }
};

// Struct for holding x,y coordinates
struct XYCoordinate {
    uint8_t x;
    uint8_t y;

    // Default Constructor. Initializes coordinates to 0,0
    XYCoordinate();

    // Constructor with coordinates
    XYCoordinate(const uint8_t i_x, const uint8_t i_y);

    // Comparison Operator
    bool operator==(const XYCoordinate &i_coordinate) const;

    //Copy operator
    XYCoordinate& operator= (const XYCoordinate &i_coordinate);
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

// Function to get adjacent pieces
// max = grid size. IE, 8 for an 8x8 grid
inline std::vector<XYCoordinate> get_adjacent(const XYCoordinate &i_coordinate, uint8_t max) {
    std::vector<XYCoordinate> result;
    // Adjust max for 0 origin
    max -= 1;
    // if x > 0, add left
    if (i_coordinate.x > 0) {
        result.push_back(XYCoordinate(i_coordinate.x - uint8_t(1), i_coordinate.y));
    }
    // if x < max, add right
    if (i_coordinate.x < max) {
        result.push_back(XYCoordinate(i_coordinate.x + uint8_t(1), i_coordinate.y));
    }
    // if y > 0, add down
    if (i_coordinate.y > 0) {
        result.push_back(XYCoordinate(i_coordinate.x, i_coordinate.y - uint8_t(1)));
    }
    // if y < max, add up
    if (i_coordinate.y < max) {
        result.push_back(XYCoordinate(i_coordinate.x, i_coordinate.y + uint8_t(1)));
    }
    return result;
}

// Function to check adjacent pieces
// max = grid size. IE, 8 for an 8x8 grid
// Returns true if coordinates are adjacent
inline bool check_adjacent(const XYCoordinate &i_coordinate_1, const XYCoordinate &i_coordinate_2, uint8_t max) {
    // Take coordinate_1 and see if coordinate_2 is one of the adjacent grids
    std::vector<XYCoordinate> adjacent_coordinates = get_adjacent(i_coordinate_1, max);

    return (std::find(adjacent_coordinates.begin(), adjacent_coordinates.end(), i_coordinate_2) != adjacent_coordinates.end());
}

// Class for holding a Go string (series of adjacently connected stones of the same color).
class GoString {
private:
    // String member coordinates
    std::vector<XYCoordinate> members;

    // Coordinates providing liberty
    std::vector<XYCoordinate> liberty;

    // Board size for String
    uint8_t board_size;
public:
    // Default constructor
    GoString(uint8_t i_board_size);

    // Destructor
    virtual ~GoString();

    // Comparison Operator
    bool operator==(const GoString &i_string) const;

    //Copy operator
    GoString& operator= (const GoString &i_string);

    // Function for appending a member to the string. Returns true on success, or false on failure
    bool append_member(const XYCoordinate &coordinate);

    // Function for appending liberty to the string. Returns true on success, or false on failure
    bool append_liberty(const XYCoordinate &coordinate);

    // Function to get the current members
    const std::vector<XYCoordinate> get_members() const;

    // Function to get the current liberty
    const std::vector<XYCoordinate> get_liberty() const;

    // Function to get the member count
    const uint8_t get_member_count() const;

    // Function to get the liberty count
    const uint8_t get_liberty_count() const;

    // Function to get the board size
    const uint8_t get_size() const;
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

    // Comparison Operator
    bool operator==(const Move &i_move) const;

    //Copy operator
    Move& operator= (const Move &i_move);

    // Check to make sure coordinates are within the bounds of the Go Board.
    // Returns true if within bounds. Otherwise, false.
    const inline bool within_bounds(const uint8_t x, const uint8_t y) const;

    // Function to construct a string given an existing string
    const GoString construct_string(GoString i_string, const bool color) const;

    // Function to remove a string from the board
    // Returns true on success
    bool remove_string(const GoString &i_string, const bool color);

    // Determines the impact of a move on the board (modifies board). Returns -1 if the piece has already been placed, otherwise
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

    // Flag to determine if move_list is dirty
    bool move_list_dirty;

    // Flag to determine last move_list color
    bool move_list_color;
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
    // Throws GoBoardBadMove exception if move is not valid
    void make_move(const Move &i_move);
};


#endif //SCALABLE_GO_GOBOARD_H
