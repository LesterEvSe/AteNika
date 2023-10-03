#include "move.hpp"

bool operator< (const Move &left, const Move &right) {
    return left.m_evaluation < right.m_evaluation;
}
