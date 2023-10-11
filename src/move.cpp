#include "move.hpp"

bool operator<(const Move &left, const Move &right) {
    return left.m_score < right.m_score;
}

int32_t Move::get_score() const {
    return m_score;
}
