#include "qmovepicker.hpp"
#include "mvv_lva.hpp"
#include "ttable.hpp"

QMovePicker::QMovePicker(MoveList *move_list, const ZobristHash &hash):
    m_move_list(*move_list), m_curr_node(0), m_size(0)
{
    Move best_move;
    if (Ttable::in_table(hash))
        best_move = Ttable::get(hash);
    else
        best_move = Move();

    for (uint8_t i = 0; i < m_move_list.size(); ++i) {
        if (m_move_list[i] == best_move) {
            m_move_list[i].set_score(INF);
            continue;
        }

        switch(m_move_list[i].get_flag()) {
            case Move::CAPTURE_PROMOTION:
                m_move_list[i].set_score(MvvLva::PROMOTION_BONUS + MvvLva::CAPTURE_BONUS +
                    MvvLva::mvv_lva[m_move_list[i].get_captured_piece()][m_move_list[i].get_move_piece()]
                ); ++m_size; break;

            case Move::EN_PASSANT:
            case Move::CAPTURE:
                m_move_list[i].set_score(MvvLva::CAPTURE_BONUS +
                    MvvLva::mvv_lva[m_move_list[i].get_captured_piece()][m_move_list[i].get_move_piece()]
                ); ++m_size;
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

    for (uint8_t i = m_curr_node+1; i < m_move_list.size(); ++i) {
        if (score < m_move_list[i].get_score()) {
            score = m_move_list[i].get_score();
            max_val_ind = i;
        }
    }

    Move temp = m_move_list[m_curr_node];
    m_move_list[m_curr_node] = m_move_list[max_val_ind];
    m_move_list[max_val_ind] = temp;
    return m_move_list[m_curr_node++];
}