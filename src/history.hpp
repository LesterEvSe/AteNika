#ifndef ATENICA_HISTORY_HPP
#define ATENICA_HISTORY_HPP

#include "zobrist_hash.hpp"

class History {
private:
    uint8_t m_size;
    bits96 m_history[DRAW_RULE_50+1]; // TODO maybe we can use it without extra one

public:
    History() : m_size(0) {}

    void add_pos(const bits96 &hash);
    void clear();

    [[nodiscard]] bool rule_of_threes(const bits96 &hash) const;
    [[nodiscard]] bool without_incident_50_moves() const;
};

#endif //ATENICA_HISTORY_HPP
