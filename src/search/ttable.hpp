// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include <cstdint>
#include <unordered_map>

#include "core/move.hpp"
#include "core/zobrist_hash.hpp"

// NONE   default, zero slot.
// ALPHA  no move raised alpha
// EXACT  alpha < score < beta
// BETA   a move failed high.
//
// enum class to prevent collision with PieceType::NONE
enum class TTFlag : uint8_t { NONE, ALPHA, EXACT, BETA };

struct TTEntry {
  Move move;
  int32_t score;
  int16_t depth;
  TTFlag flag;
};

namespace TTable {
  void add(const ZobristHash &zob, TTEntry entry);
  [[nodiscard]] bool in_table(const ZobristHash &zob);
  [[nodiscard]] const TTEntry &get(const ZobristHash &zob);

  // The table currently outlives every search and every game. bench clears it
  // between positions so a run depends only on the binary, not on whatever was
  // searched before it. Also what UCI "ucinewgame" will need in Phase 0C.
  void clear();
} // namespace TTable

namespace TTable::detail {
  extern std::unordered_map<uint64_t, TTEntry> _ttable;

} // namespace TTable::detail
