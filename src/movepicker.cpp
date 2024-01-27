#include "movepicker.hpp"
#include "mvv_lva.hpp"
#include "board.hpp"
#include "eval.hpp"
#include "Ttable.hpp"

MovePicker::MovePicker(MoveList *move_list, const Board &board) :
    m_move_list(*move_list), m_curr_node(0)
{
    ZobristHash zob_hash = board.get_zob_hash();
    Move best_move = Move();

    if (Transposition::in_table(zob_hash))
        best_move = Transposition::get(zob_hash).get_best_move();

    Color curr = board.get_curr_move();
    for (uint8_t i = 0; i < m_move_list.size(); ++i) {
        if (best_move == m_move_list[i])
            m_move_list[i].set_score(INF);
        else {
            int32_t extra = m_move_list[i].get_flag() == Move::QSIDE_CASTLING ||
                m_move_list[i].get_flag() == Move::KSIDE_CASTLING ? 50 : 0;
            m_move_list[i].set_score(Eval::evaluate<Eval::FAST>(board, curr) + extra);
        }
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