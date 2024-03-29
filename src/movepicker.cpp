#include "movepicker.hpp"
#include "mvv_lva.hpp"
#include "board.hpp"
#include "eval.hpp"
#include "Ttable.hpp"

MovePicker::MovePicker(MoveList *move_list, const Board &board, const OrderInfo &order_info) :
    m_move_list(*move_list), m_curr_node(0)
{
    ZobristHash zob_hash = board.get_zob_hash();
    Move best_move = Move();

    if (Transposition::in_table(zob_hash))
        best_move = Transposition::get(zob_hash).get_best_move();

    for (uint8_t i = 0; i < m_move_list.size(); ++i) {
        if (best_move == m_move_list[i]) {
            m_move_list[i].set_score(INF);
            continue;
        }

        int32_t score = 0;
        switch (m_move_list[i].get_flag()) {
            case Move::CAPTURE_PROMOTION:
                score += MvvLva::PROMOTION_BONUS + Eval::get_material(m_move_list[i].get_promotion_piece());
            // planned fallthrough
            case Move::CAPTURE:
                score += MvvLva::CAPTURE_BONUS + MvvLva::mvv_lva[m_move_list[i].get_move_piece()][m_move_list[i].get_captured_piece()];
                break;
            case Move::PROMOTION:
                score += MvvLva::PROMOTION_BONUS + Eval::get_material(m_move_list[i].get_promotion_piece());
                break;
            default:
                score += order_info.get_history(board.get_curr_move(), m_move_list[i].get_from_cell(), m_move_list[i].get_to_cell());
                break;
        }
        if (m_move_list[i] == order_info.get_killer1())
            score += OrderInfo::KILLER1_BONUS;
        else if (m_move_list[i] == order_info.get_killer2())
            score += OrderInfo::KILLER2_BONUS;
        m_move_list[i].set_score(score);
    }
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

    Move temp = m_move_list[m_curr_node];
    m_move_list[m_curr_node] = m_move_list[max_val_ind];
    m_move_list[max_val_ind] = temp;

    return m_move_list[m_curr_node++];
}