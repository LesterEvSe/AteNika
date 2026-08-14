// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include "search/order_info.hpp"

#include <cstring> // for std::memset

// killers will be initialized automatically
OrderInfo::OrderInfo() : m_ply(0) { std::memset(m_history, 0, sizeof(m_history)); }

void OrderInfo::operator++() { ++m_ply; }
void OrderInfo::operator--() { --m_ply; }

void OrderInfo::add_history(uint8_t from, uint8_t to, int16_t depth) {
  m_history[from][to] += depth;

  // TODO: Need to be checked with sprt.
  // Unbounded, and the table is global from-to with no piece or color split, so
  // a hot pair can cross KILLER bonus in a long search and start outranking the killers.
  // if (m_history[from][to] >= 50'000)
  //   for (auto &row : m_history)
  //     for (int32_t &value : row)
  //       value /= 2;
}

void OrderInfo::add_killer(Move move) {
  m_killers2[m_ply] = m_killers1[m_ply];
  m_killers1[m_ply] = move;
}

Move OrderInfo::get_killer1() const { return m_killers1[m_ply]; }
Move OrderInfo::get_killer2() const { return m_killers2[m_ply]; }

int32_t OrderInfo::get_history(uint8_t from, uint8_t to) const { return m_history[from][to]; }
int16_t OrderInfo::get_ply() const { return m_ply; }