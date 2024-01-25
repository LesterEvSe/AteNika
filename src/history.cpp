#include "history.hpp"

void History::operator=(const History &history) {
    m_size = history.m_size;
    for (uint8_t i = 0; i < m_size; ++i)
        m_history[i] = history.m_history[i];
}

void History::add_pos(const ZobristHash &zob_hash) {
    m_history[m_size++] = (bits96)zob_hash.get_hash();
}

void History::clear() {
    m_size = 0;
}

bool History::rule_of_threes(const ZobristHash &zob_hash) const {
    uint8_t repetitions = 0;
    bits96 hash = (bits96)zob_hash.get_hash();

    for (uint8_t i = 0; i < m_size; ++i)
        if (m_history[i] == hash)
            ++repetitions;
    return repetitions >= 3;
}