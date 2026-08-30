// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include "search/movepicker.hpp"

#include "search/mvv_lva.hpp"

MovePicker::MovePicker(Color color, MoveList *move_list, const Move &tt_move, const Move &prev_move,
                       OrderInfo &order_info)
    : m_move_list(*move_list), m_curr_node(0) {
  for (uint8_t i = 0; i < m_move_list.size(); ++i) {
    if (m_move_list[i] == tt_move) {
      m_move_list.set_score(i, INF);
      continue;
    }

    // Warning!!! The value is not initialized, so we need to set up it later
    int32_t score;
    switch (m_move_list[i].get_flag()) {
      case Move::CAPTURE_PROMOTION:
        score =
            MvvLva::PROMOTION_BONUS + MvvLva::CAPTURE_BONUS +
            MvvLva::mvv_lva[m_move_list[i].get_captured_piece()][m_move_list[i].get_move_piece()];
        break;
      case Move::EN_PASSANT:
      case Move::CAPTURE:
        score =
            MvvLva::CAPTURE_BONUS +
            MvvLva::mvv_lva[m_move_list[i].get_captured_piece()][m_move_list[i].get_move_piece()];
        break;
      case Move::PROMOTION: score = MvvLva::PROMOTION_BONUS; break;
      default:
        if (m_move_list[i] == order_info.get_killer1())
          score = OrderInfo::KILLER1_BONUS;
        else if (m_move_list[i] == order_info.get_killer2())
          score = OrderInfo::KILLER2_BONUS;
        else
          // Their sum still less than KILLER2_BONUS.
          score = order_info.get_history(color, m_move_list[i].get_from_cell(),
                                         m_move_list[i].get_to_cell()) +
                  order_info.get_cont_hist(color, prev_move, m_move_list[i]);
        break;
    }
    m_move_list.set_score(i, score);
  }
}

bool MovePicker::has_next() const { return m_curr_node < m_move_list.size(); }

const Move &MovePicker::get_next() {
  int32_t score = m_move_list.get_score(m_curr_node);
  uint8_t max_val_ind = m_curr_node;

  for (uint8_t i = m_curr_node + 1; i < m_move_list.size(); ++i) {
    if (score < m_move_list.get_score(i)) {
      score = m_move_list.get_score(i);
      max_val_ind = i;
    }
  }

  Move temp_move = m_move_list[m_curr_node];
  int32_t temp_score = m_move_list.get_score(m_curr_node);

  m_move_list[m_curr_node] = m_move_list[max_val_ind];
  m_move_list.set_score(m_curr_node, score);

  m_move_list[max_val_ind] = temp_move;
  m_move_list.set_score(max_val_ind, temp_score);

  return m_move_list[m_curr_node++];
}