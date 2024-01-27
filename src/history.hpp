#ifndef ATENIKA_HISTORY_HPP
#define ATENIKA_HISTORY_HPP

#include "zobrist_hash.hpp"

class History {
private:
    int16_t m_size;
    bits96 m_history[DRAW_RULE_50];

public:
    History() : m_size(0) {}
    void operator=(const History &history);

    bool add_pos(const ZobristHash &zob_hash);
    void clear();
    [[nodiscard]] bool rule_of_threes(const ZobristHash &zob_hash) const;
};

#endif //ATENIKA_HISTORY_HPP
