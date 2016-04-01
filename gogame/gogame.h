// Copyright [2016] <duncan@wduncanfraser.com>
// Prototypes for GoGame and related classes

#ifndef GOGAME_GOGAME_H_
#define GOGAME_GOGAME_H_

#include <algorithm>
#include <array>
#include <vector>
#include <unordered_set>
#include <cstdint>
#include <stdexcept>

#define BLACK_MASK 1
#define WHITE_MASK 3
#define PIECE_MASK 1
#define TEAM_MASK 3
#define SCORED_MASK 4

// GoGame exceptions
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

class GoBoardBadMask : public std::runtime_error {
public:
    GoBoardBadMask() : std::runtime_error("GoBoardBadMask") { }
};

// Class for holding x,y coordinates
class XYCoordinate {
 public:
    uint8_t x;
    uint8_t y;

    // Default Constructor. Initializes coordinates to 0,0
    XYCoordinate();

    // Constructor with coordinates
    XYCoordinate(const uint8_t i_x, const uint8_t i_y);

    // Comparison Operator
    bool operator==(const XYCoordinate &i_coordinate) const;

    // Copy operator
    XYCoordinate& operator= (const XYCoordinate &i_coordinate);
};

// Helper functions
// Function to get piece mask in int format from bool
// black = 0, white = 1
inline uint8_t get_mask(const bool color) {
    if (color) {
        return uint8_t(WHITE_MASK);
    } else {
        return uint8_t(BLACK_MASK);
    }
}

// Function to get bool from piece mask
inline bool get_piece_bool(const uint8_t mask) {
    if ((mask & TEAM_MASK) == BLACK_MASK) {
        return 0;
    } else if ((mask & TEAM_MASK) == WHITE_MASK) {
        return 1;
    } else {
        throw GoBoardBadMask();
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

    return (std::find(adjacent_coordinates.begin(),
                      adjacent_coordinates.end(), i_coordinate_2) != adjacent_coordinates.end());
}

// Struct for holding a Go Board
class GoBoard {
 public:
    // Go board
    // TODO(wdfraser): Make this private with [] operator
    std::vector<std::vector<uint8_t>> board;

    // Constructor with size specification
    explicit GoBoard(const uint8_t board_size);

    // Constructor with board passed as a vector
    explicit GoBoard(const std::vector<std::vector<uint8_t>> &i_board);

    // Copy Constructor
    GoBoard(const GoBoard &i_goboard);

    // Comparison Operator
    bool operator==(const GoBoard &i_goboard) const;

    // Copy operator
    GoBoard& operator= (const GoBoard &i_goboard);

    // Function to get the board size
    const uint8_t get_size() const;

    // Check to make sure coordinates are within the bounds of the Go Board.
    // Returns true if within bounds. Otherwise, false.
    const inline bool within_bounds(const XYCoordinate &i_piece) const;
};

// Class for holding a Go string (series of adjacently connected stones of the same color).
class GoString {
 private:
    // String member coordinates
    std::vector<XYCoordinate> members;

    // Coordinates providing liberty
    // Not used in territory strings
    std::vector<XYCoordinate> liberty;

    // Board size for String
    uint8_t board_size;

    // Value determining border of territory strings
    // 0 = neutral, BLACK_MASK = black, WHITE_MASK = white.
    uint8_t territory_border;

 public:
    // Default constructor
    explicit GoString(uint8_t i_board_size);

    // Destructor
    virtual ~GoString();

    // Comparison Operator
    bool operator==(const GoString &i_string) const;

    // Copy operator
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

    // Function to set the territory border
    // 0 = neutral, BLACK_MASK = black, WHITE_MASK = white.
    void set_border(uint8_t color);

    // Function to get the teritory border
    const uint8_t get_border() const;
};

// Class for holding a possible move
class GoMove {
    friend class GoGame;
 private:
    // Resultant Game Board
    GoBoard goboard;

    // Piece placed
    XYCoordinate piece;

    // Count of prisoners captures
    uint8_t prisoners_captured;

    // Flag to determine if the move is a pass
    bool pass;

 public:
    // Pass Constructor, takes the current board only
    explicit GoMove(const GoBoard &i_goboard);

    // Standard Constructor, takes the existing board state and the piece to be placed
    GoMove(const GoBoard &i_goboard, const XYCoordinate &i_piece);

    // Comparison Operator
    bool operator==(const GoMove &i_move) const;

    // Copy operator
    GoMove& operator= (const GoMove &i_move);

    // Function to construct a string given an existing string
    const GoString construct_string(GoString i_string, const bool color) const;

    // Function to remove a string from the board
    // Returns true on success
    bool remove_string(const GoString &i_string, const bool color);

    // Determines the impact of a move on the board (modifies board). Returns -1 if the piece has already been placed,
    // otherwise returns the liberty of the piece/string.
    // Pass moves return 0.
    // black = 0, white = 1
    int check_move(const bool color);

    // Function to get board
    const GoBoard get_board() const;

    // Function to get piece
    const XYCoordinate get_piece() const;

    // Function to get prisoner count
    const uint8_t get_prisoners_captured() const;

    // Function to check if hte move is a pass.
    const bool check_pass() const;
};

class GoGame {
 private:
    // Game board
    GoBoard goboard;

    // GoMove list. Cached list of possible moves from current board state.
    std::vector<GoMove> move_list;

    // GoMove history
    // TODO(wdfraser): Let's make this a hash/unordered_set for faster lookup
    std::vector<GoMove> move_history;

    // Flag to determine if move_list is dirty
    bool move_list_dirty;

    // Flag to determine last move_list color
    bool move_list_color;

    // Array to hold prisoners.
    // prisoner_count[0] = black
    // prisoner_count[1] = white
    std::array<uint8_t, 2> prisoner_count;

    // Array to hold pieces placed count
    std::array<uint8_t, 2> pieces_placed;

 public:
    // Constructor with size specification
    explicit GoGame(const uint8_t board_size);

    // Constructor with board passed as a vector
    explicit GoGame(const GoBoard &i_goboard);

    // Copy Constructor
    GoGame(const GoGame &i_gogame);

    // Comparison Operator
    bool operator==(const GoGame &i_gogame) const;

    // Function to get the board size
    const uint8_t get_size() const;

    // Function to get current board state
    const GoBoard get_board() const;

    // Function to set the board. Overrides all checks and should only be used for testing
    void set_board(const GoBoard &i_goboard);

    // Function to get the move list
    const std::vector<GoMove> get_move_list() const;

    // Function to get the prisoner counts
    const std::array<uint8_t, 2> get_prisoner_count() const;

    // Function to get the pieces placed count
    const std::array<uint8_t, 2> get_pieces_placed() const;

    // Checks if move has been made before
    // Returns true if board state has previously existed.
    const bool check_move_history(const GoMove &i_move) const;

    // Generate all possible moves for specified color.
    // black = 0, white = 1
    // Return value of 0 signifies no moves.
    bool generate_moves(const bool color);

    // Makes a move
    // Throws GoBoardBadMove exception if move is not valid
    void make_move(const GoMove &i_move, const bool color);

    // Construct a territory string
    const GoString construct_territory_string(GoString i_string) const;

    // Calculates current scores. First value is black score. Second is white.
    // Using territory scoring with no seki.
    // Territory is calculated as a string of empty spaces surrounded by only a single color.
    // Prisoner count is added to territory score.
    const std::array<uint8_t, 2> calculate_scores() const;
};


#endif  // GOGAME_GOGAME_H_
