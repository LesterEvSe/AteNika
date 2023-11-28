#ifndef ATENICA_HISTORY_HPP
#define ATENICA_HISTORY_HPP

#include "zobrist_hash.hpp"

class History {
private:
    int16_t m_size;
    bits96 m_history[DRAW_RULE_50];

public:
    History() : m_size(0) {}

    void add_pos(const ZobristHash &zob_hash);
    void clear();
    [[nodiscard]] bool rule_of_threes(const ZobristHash &zob_hash) const;
};

#endif //ATENICA_HISTORY_HPP
