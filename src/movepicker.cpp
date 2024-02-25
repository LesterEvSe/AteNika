#include "movepicker.hpp"
#include "mvv_lva.hpp"
#include "ttable.hpp"

MovePicker::MovePicker(MoveList *move_list, const ZobristHash &hash, OrderInfo &order_info):
    m_move_list(*move_list), m_curr_node(0)
{
    Move best_move;
    if (TTable::in_table(hash))
        best_move = TTable::get(hash).move;
    else
        best_move = Move();

    for (uint8_t i = 0; i < m_move_list.size(); ++i) {
        if (m_move_list[i] == best_move) {
            m_move_list[i].set_score(INF);
            continue;
        }

        // Warning!!! The value is not initialized, so we need to reset it later
        // score =
        int32_t score;
        switch (m_move_list[i].get_flag()) {
            case Move::CAPTURE_PROMOTION:
                score = MvvLva::PROMOTION_BONUS + MvvLva::CAPTURE_BONUS + MvvLva::mvv_lva[m_move_list[i].get_captured_piece()][m_move_list[i].get_move_piece()];
                break;
            case Move::EN_PASSANT:
            case Move::CAPTURE:
                score = MvvLva::CAPTURE_BONUS + MvvLva::mvv_lva[m_move_list[i].get_captured_piece()][m_move_list[i].get_move_piece()];
                break;
            case Move::PROMOTION:
                score = MvvLva::PROMOTION_BONUS;
                break;
            default:
                if (m_move_list[i] == order_info.get_killer1())
                    score = OrderInfo::KILLER1_BONUS;
                else if (m_move_list[i] == order_info.get_killer2())
                    score = OrderInfo::KILLER2_BONUS;
                else
                    score = order_info.get_history(m_move_list[i].get_from_cell(), m_move_list[i].get_to_cell());
                break;
        }
        m_move_list[i].set_score(score);
    }
}

bool MovePicker::has_next() const {
    return m_curr_node < m_move_list.size();
}

const Move &MovePicker::get_next() {
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