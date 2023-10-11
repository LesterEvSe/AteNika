#ifndef CHESSAI_MOVE_LIST_HPP
#define CHESSAI_MOVE_LIST_HPP

#include "move.hpp"

class MovePicker {
private:
    uint8_t m_size;
    uint8_t m_curr_node; // First unpicked move

    // This is the position with the max number of moves a player can have
    Move m_moves[218];

    // MVV - Most Valuable Victim
    // LVA - Least Valuable Attacker
    static uint8_t m_mvv_lva[PSIZE-1][PSIZE];

public:
    static void init();

    MovePicker() : m_size(0), m_curr_node(0), m_moves(Move()) {}
    [[nodiscard]] bool has_next() const;
    [[nodiscard]] uint8_t get_moves() const;

    const Move &get_next(); // Using insertion sorting in place, O(n) at a time
    void emplace_back(const Move &move);
};


#endif //CHESSAI_MOVE_LIST_HPP
