#include <algorithm> // for std::swap
#include "movepicker.hpp"

uint8_t MovePicker::m_mvv_lva[PIECE_SIZE - 1][PIECE_SIZE];

MovePicker::MovePicker(MoveList *move_list) :
    m_move_list(*move_list), m_curr_node(0)
{
    for (uint8_t i = 0; i < m_move_list.size(); ++i) {
        int32_t score = 0;
        switch (m_move_list[i].get_flag()) {
            case Move::CAPTURE_PROMOTION:
                score += PROMOTION_BONUS;
            case Move::CAPTURE:
                score += m_mvv_lva[m_move_list[i].get_move_piece()][m_move_list[i].get_captured_piece()];
                score += CAPTURE_BONUS;
                break;
            case Move::PROMOTION:
                score += PROMOTION_BONUS;
            default:
                break;
        }
        m_move_list[i].set_score(score);
    }
}

void MovePicker::init() {
    PieceType MVV[PIECE_SIZE - 1] = {PAWN, KNIGHT, BISHOP, ROOK, QUEEN };
    PieceType LVA[PIECE_SIZE] = {KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };

    uint8_t value = 0;
    for (PieceType victim : MVV)
        for (PieceType attacker : LVA)
            m_mvv_lva[victim][attacker] = ++value;
}

bool MovePicker::has_next() const {
    return m_curr_node < m_move_list.size();
}

const Move &MovePicker::get_next() {
    int32_t score = m_move_list[m_curr_node].get_score();
    uint8_t max_val_ind = m_curr_node;

    for (uint8_t i = m_curr_node+1; i < m_move_list.size(); ++i)
        if (score < m_move_list[i].get_score()) {
            score = m_move_list[i].get_score();
            max_val_ind = i;
        }

    std::swap(m_move_list[m_curr_node], m_move_list[max_val_ind]);
    return m_move_list[m_curr_node++];
}