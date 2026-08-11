// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include "core/move_list.hpp"
#include "core/zobrist_hash.hpp"

class QMovePicker {
private:
  MoveList &m_move_list;
  uint8_t m_curr_node;
  uint8_t m_size;

public:
  explicit QMovePicker(MoveList *move_list, const ZobristHash &hash);
  [[nodiscard]] bool has_next() const;
  const Move &get_next();
};
