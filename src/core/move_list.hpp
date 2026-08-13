// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include "core/move.hpp"

class MoveList {
private:
  uint8_t m_size;

  // This is the position with the max number of moves a player can have
  Move m_moves[218];
  int32_t m_scores[218];

public:
  MoveList() : m_size(0), m_moves{Move()} {}
  [[nodiscard]] uint8_t size() const;

  // Only 4 bytes, so can just copy it.
  // & is need for m_move_list[i].set_score in movepicker constructor.
  Move &operator[](uint8_t ind);
  void emplace_back(const Move &move);

  [[nodiscard]] int32_t get_score(uint8_t ind) const;
  void set_score(uint8_t ind, int32_t val);
};
