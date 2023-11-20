#include "history.hpp"

void History::add_pos(const bits96 &hash) {
    m_history[m_size++] = hash;
}

void History::clear() {
    m_size = 0;
}

bool History::rule_of_threes(const bits96 &hash) const {
    uint8_t repetitions = 0;
    for (uint8_t i = 0; i < m_size; ++i)
        if (m_history[i] == hash)
            ++repetitions;
    return repetitions >= 3;
}

bool History::without_incident_50_moves() const {
    return m_size >= 50;
}