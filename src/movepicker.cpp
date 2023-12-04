#include "movepicker.hpp"
#include "mvv_lva.hpp"
#include "board.hpp"
#include "pst.hpp"

MovePicker::MovePicker(MoveList *move_list, const Board &board) :
    m_move_list(*move_list), m_curr_node(0)
{
    Color move = board.get_curr_move();
    for (uint8_t i = 0; i < m_move_list.size(); ++i) {

        int32_t score = 0;
        PieceTables pst = board.get_pst();
        PieceType piece = m_move_list[i].get_move_piece();

        pst.remove_piece(move, piece, m_move_list[i].get_from_cell());
        pst.add_piece(move, piece, m_move_list[i].get_to_cell());

        switch (m_move_list[i].get_flag()) {
            case Move::CAPTURE_PROMOTION:
                score += MvvLva::PROMOTION_BONUS;
                // TODO change pst
            case Move::CAPTURE:
                score += MvvLva::mvv_lva[m_move_list[i].get_move_piece()][m_move_list[i].get_captured_piece()];
                score += MvvLva::CAPTURE_BONUS;
                // and here
                break;
            case Move::PROMOTION:
                score += MvvLva::PROMOTION_BONUS;
                // and here as at first
            default:
                break;
        }

        score += pst.get_eval(board.get_curr_move(), OPENING);
        score -= pst.get_eval(board.get_opponent_move(), OPENING);
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

    // TODO maybe have a problem with swap
    Move temp = m_move_list[m_curr_node];
    m_move_list[m_curr_node] = m_move_list[max_val_ind];
    m_move_list[max_val_ind] = temp;

    return m_move_list[m_curr_node++];
}