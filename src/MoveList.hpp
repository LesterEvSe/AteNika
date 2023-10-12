#ifndef ATENICA_MOVELIST_HPP
#define ATENICA_MOVELIST_HPP

#include "move.hpp"

class MoveList {
private:
    uint8_t m_size;
    Move m_moves[218];

public:
    MoveList() : m_size(0), m_moves(Move()) {}
    [[nodiscard]] uint8_t size() const;
    Move &operator[](uint8_t index);
};


#endif //ATENICA_MOVELIST_HPP
