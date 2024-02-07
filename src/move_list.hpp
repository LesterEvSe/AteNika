#ifndef ATENIKA_MOVE_LIST_HPP
#define ATENIKA_MOVE_LIST_HPP

#include "move.hpp"

class MoveList {
private:
    uint8_t m_size;

    // This is the position with the max number of moves a player can have
    Move m_moves[218];

public:
    MoveList() : m_size(0), m_moves{ Move() } {}
    [[nodiscard]] uint8_t size() const;

    // & is need for m_move_list[i].set_score in movepicker constructor
    Move &operator[](uint8_t index);
    void emplace_back(const Move &move);
};

#endif //ATENIKA_MOVE_LIST_HPP
