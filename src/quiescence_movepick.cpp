#include "quiescence_movepick.hpp"
#include "mvv_lva.hpp"

QMovePicker::QMovePicker(MoveList *move_list) :
    m_move_list(*move_list), m_curr_node(0)
{
    for (uint8_t i = 0; i < m_move_list.size(); ++i) {
        int32_t score = 0;
        switch (m_move_list[i].get_flag()) {
            case Move::CAPTURE_PROMOTION:
                score += MvvLva::PROMOTION_BONUS;
            case Move::CAPTURE:
                score += MvvLva::mvv_lva[m_move_list[i].get_move_piece()][m_move_list[i].get_captured_piece()];
                score += MvvLva::CAPTURE_BONUS;
                break;
            case Move::PROMOTION:
                score += MvvLva::PROMOTION_BONUS;
            default:
                break;
        }

        // Set moves only with captures or promotions
        if (score != 0) {
            m_move_list[m_size] = m_move_list[i];
            m_move_list[m_size++].set_score(score);
        }
    }
}

bool QMovePicker::has_next() const {
    m_curr_node < m_size;
}

const Move &QMovePicker::get_next() {
    int32_t score = m_move_list[m_curr_node].get_score();
    uint8_t max_val_ind = m_curr_node;

    for (uint8_t i = m_curr_node+1; i < m_size; ++i)
        if (score < m_move_list[i].get_score()) {
            score = m_move_list[i].get_score();
            max_val_ind = i;
        }

    // TODO maybe have a problem with swap
    Move temp = m_move_list[m_curr_node];
    m_move_list[m_curr_node] = m_move_list[max_val_ind];
    m_move_list[max_val_ind] = temp;

    return m_move_list[m_curr_node++];
}