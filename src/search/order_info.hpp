// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include "core/move.hpp"
#include "defs.hpp"

// Can skip killer check, because we do it inside search.cpp file.
class OrderInfo {
private:
  int16_t m_ply;

  // https://www.chessprogramming.org/History_Heuristic
  // https://www.chessprogramming.org/Butterfly_Boards
  int32_t m_history[COLOR_SIZE][64][64]; // color, from square, to square
  Move m_killers1[MAX_SEARCH_PLY], m_killers2[MAX_SEARCH_PLY];

public:
  static constexpr int32_t KILLER1_BONUS = 200'000;
  static constexpr int32_t KILLER2_BONUS = 100'000;

  // The gravity can be in range +/- MAX_HISTORY, but not out of that,
  // not to override KILLERs
  static constexpr int32_t MAX_HISTORY = 16'384;
  static constexpr int32_t MAX_BONUS = 1'200;

  OrderInfo();
  void operator++();
  void operator--();

  // Everything a single search owns: ply, killers and the history table.
  void new_search();
  void new_game();

  void add_killer(Move move);
  void add_history(Color color, uint8_t from, uint8_t to, int16_t depth);

  [[nodiscard]] Move get_killer1() const;
  [[nodiscard]] Move get_killer2() const;

  [[nodiscard]] int32_t get_history(Color color, uint8_t from, uint8_t to) const;
  [[nodiscard]] int16_t get_ply() const;
};
