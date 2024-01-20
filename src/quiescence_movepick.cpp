#include "quiescence_movepick.hpp"
#include "mvv_lva.hpp"
#include "eval.hpp"

QMovePicker::QMovePicker(MoveList *move_list) :
    m_move_list(*move_list), m_curr_node(0), m_size(0)
{
    for (uint8_t i = 0; i < m_move_list.size(); ++i) {
        int32_t score = 0;
        switch (m_move_list[i].get_flag()) {
            case Move::CAPTURE_PROMOTION:
                score += MvvLva::PROMOTION_BONUS + Eval::get_material(m_move_list[i].get_promotion_piece());
            case Move::CAPTURE:
                score += MvvLva::CAPTURE_BONUS + MvvLva::mvv_lva[m_move_list[i].get_move_piece()][m_move_list[i].get_captured_piece()];
                ++m_size;
                m_move_list[i].set_score(score);
                break;
            case Move::PROMOTION:
                score += MvvLva::PROMOTION_BONUS + Eval::get_material(m_move_list[i].get_promotion_piece());
                m_move_list[i].set_score(score);
            default:
                break;
        }
    }
}

bool QMovePicker::has_next() const {
    return m_curr_node < m_size;
}

const Move &QMovePicker::get_next() {
    int32_t score = m_move_list[m_curr_node].get_score();
    uint8_t max_val_ind = m_curr_node;

    for (uint8_t i = m_curr_node+1; i < m_move_list.size(); ++i)
        if ((m_move_list[i].get_flag() & Move::CAPTURE_PROMOTION) && score < m_move_list[i].get_score()) {
            score = m_move_list[i].get_score();
            max_val_ind = i;
        }

    Move temp = m_move_list[m_curr_node];
    m_move_list[m_curr_node] = m_move_list[max_val_ind];
    m_move_list[max_val_ind] = temp;

    return m_move_list[m_curr_node++];
}