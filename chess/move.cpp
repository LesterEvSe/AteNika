#include "move.hpp"

bool operator< (const Move &left, const Move &right) {
    return left.m_estimation < right.m_estimation;
}

bool operator> (const Move &left, const Move &right) {
    return left.m_estimation > right.m_estimation;
}