// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include "core/move_list.hpp"
#include "search/order_info.hpp"

class MovePicker {
private:
  MoveList &m_move_list;
  uint8_t m_curr_node;

public:
  // Takes the transposition-table move rather than the key: the caller already
  // probed to decide whether it could cut, so probing again here would be a
  // second lookup for an answer it is holding.
  explicit MovePicker(MoveList *move_list, const Move &tt_move, OrderInfo &order_info);
  [[nodiscard]] bool has_next() const;
  const Move &get_next(); // Using selection sorting in place, O(n) at a time
};
