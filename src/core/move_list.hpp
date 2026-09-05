// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include "core/move.hpp"

class MoveList {
private:
  uint8_t m_size;

  // The most legal moves any known chess position allows
  static constexpr uint32_t MAX_NUMBER_OF_MOVES = 218;

  Move m_moves[MAX_NUMBER_OF_MOVES];
  int32_t m_scores[MAX_NUMBER_OF_MOVES];

public:
  MoveList() : m_size(0) {}
  [[nodiscard]] uint8_t size() const;

  // Returns a reference because both pickers swap in place while selecting.
  Move &operator[](uint8_t ind);
  void emplace_back(const Move &move);

  [[nodiscard]] int32_t get_score(uint8_t ind) const;
  void set_score(uint8_t ind, int32_t val);
};
