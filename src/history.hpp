#ifndef ATENIKA_HISTORY_HPP
#define ATENIKA_HISTORY_HPP

#include "zobrist_hash.hpp"

class History {
private:
    // We can record moves until there is no tie for the 50-move rule
    bits96 m_history[MAX_PLY];
    uint8_t m_size;

public:
    History() : m_size(0) {}
    void operator=(const History &history);

    void clear();
    bool add_pos(const ZobristHash &zob_hash);
    [[nodiscard]] bool threefold_repetition(const ZobristHash &zob_hash) const;
};

#endif //ATENIKA_HISTORY_HPP
