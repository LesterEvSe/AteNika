#ifndef CHESSAI_MOVE_LIST_HPP
#define CHESSAI_MOVE_LIST_HPP

#include "move.hpp"

class MoveList {
private:
    // This is the position with the max number of moves a player can have
    uint8_t size;
    Move moves[218];

public:

};


#endif //CHESSAI_MOVE_LIST_HPP
