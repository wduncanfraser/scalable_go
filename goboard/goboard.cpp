//
// Created by W. Duncan Fraser on 1/25/16.
//

#include <vector>
#include <cstdint>

#include "goboard.h"

//Default constructor, blank
GoBoard::GoBoard() { }

//Constructor with size specification
GoBoard::GoBoard(const uint8_t board_size) {
    if ((board_size < 3) || (board_size > 19)) {

    }
}

