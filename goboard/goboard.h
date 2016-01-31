//
// Created by W. Duncan Fraser on 1/25/16.
//

#ifndef SCALABLE_GO_GOBOARD_H
#define SCALABLE_GO_GOBOARD_H


class GoBoard {
private:
    // Game board
    std::vector<std::vector<uint8_t>> board;
public:
    //Default Constructor
    GoBoard();

    //Constructor with size specification
    GoBoard(const uint8_t board_size);

    //Copy Constructor
    GoBoard(const GoBoard &i_goboard);

    //Destructor
    ~GoBoard();

    //Function to get current boardstate
    std::vector<std::vector<uint8_t>> get_board() const;
};


#endif //SCALABLE_GO_GOBOARD_H
