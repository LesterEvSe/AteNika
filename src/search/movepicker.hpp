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
  explicit MovePicker(Color color, MoveList *move_list, const Move &tt_move, OrderInfo &order_info);
  [[nodiscard]] bool has_next() const;
  const Move &get_next(); // Using selection sorting in place, O(n) at a time
};
