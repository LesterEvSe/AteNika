#pragma once

#include "core/move_list.hpp"
#include "core/zobrist_hash.hpp"
#include "search/order_info.hpp"

class MovePicker {
private:
  MoveList &m_move_list;
  uint8_t m_curr_node;

public:
  explicit MovePicker(MoveList *move_list, const ZobristHash &hash, OrderInfo &order_info);
  [[nodiscard]] bool has_next() const;
  const Move &get_next(); // Using selection sorting in place, O(n) at a time
};
