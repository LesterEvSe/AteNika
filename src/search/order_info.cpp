// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include "search/order_info.hpp"

#include <algorithm>
#include <cstring> // for std::memset

// killers will be initialized automatically
OrderInfo::OrderInfo() : m_ply(0) {
  std::memset(m_history, 0, sizeof(m_history));
  // std::memset(m_cont_hist, 0, sizeof(m_cont_hist));
}

void OrderInfo::operator++() { ++m_ply; }
void OrderInfo::operator--() { --m_ply; }

void OrderInfo::new_search() {
  m_ply = 0;
  std::fill(std::begin(m_killers1), std::end(m_killers1), Move());
  std::fill(std::begin(m_killers2), std::end(m_killers2), Move());
}

void OrderInfo::new_game() {
  new_search();
  std::memset(m_history, 0, sizeof(m_history));
  // std::memset(m_cont_hist, 0, sizeof(m_cont_hist));
}

/*
void OrderInfo::add_cont_hist(Color color, const Move &prev, const Move &move, int16_t depth) {
  if (prev.get_flag() == Move::NULL_MOVE)
    return;

  const int32_t bonus = std::min(int32_t{depth} * depth, MAX_BONUS);
  int32_t &entry = m_cont_hist[color][prev.get_move_piece()][prev.get_to_cell()]
                              [move.get_move_piece()][move.get_to_cell()];
  entry += bonus - entry * bonus / MAX_HISTORY;
}

int32_t OrderInfo::get_cont_hist(Color color, const Move &prev, const Move &move) const {
  if (prev.get_flag() == Move::NULL_MOVE)
    return 0;

  return m_cont_hist[color][prev.get_move_piece()][prev.get_to_cell()][move.get_move_piece()]
                    [move.get_to_cell()];
}
*/

// Gravity and setup bonus (restricted by MAX_BONUS and MAX_HISTORY)
void OrderInfo::add_history(Color color, uint8_t from, uint8_t to, int16_t depth) {
  const int32_t bonus = std::min(int32_t{depth} * depth, MAX_BONUS);
  int32_t &entry = m_history[color][from][to];
  entry += bonus - entry * bonus / MAX_HISTORY;
}

void OrderInfo::add_killer(Move move) {
  m_killers2[m_ply] = m_killers1[m_ply];
  m_killers1[m_ply] = move;
}

Move OrderInfo::get_killer1() const { return m_killers1[m_ply]; }
Move OrderInfo::get_killer2() const { return m_killers2[m_ply]; }

int32_t OrderInfo::get_history(Color color, uint8_t from, uint8_t to) const {
  return m_history[color][from][to];
}
int16_t OrderInfo::get_ply() const { return m_ply; }
