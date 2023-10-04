#include <algorithm> // for std::swap
#include "movepicker.hpp"

uint8_t MovePicker::m_mvv_lva[5][6];

void MovePicker::init() {
    PieceType MVV[5] = {PAWN, KNIGHT, BISHOP, ROOK, QUEEN };
    PieceType LVA[6] = {KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };

    uint8_t value = 0;
    for (PieceType victim : MVV)
        for (PieceType attacker : LVA)
            m_mvv_lva[victim][attacker] = ++value;
}

bool MovePicker::has_next() const {
    return m_curr_node < m_size;
}

const Move &MovePicker::get_next() {
    int32_t score = m_moves[m_curr_node].get_score();
    uint8_t max_val_ind = m_curr_node;

    for (uint8_t i = m_curr_node+1; i < m_size; ++i)
        if (score < m_moves[i].get_score()) {
            score = m_moves[i].get_score();
            max_val_ind = i;
        }

    std::swap(m_moves[m_curr_node], m_moves[max_val_ind]);
    return m_moves[m_curr_node++];
}

void MovePicker::emplace_back(const Move &move) {
    m_moves[m_size++] = move;
}
