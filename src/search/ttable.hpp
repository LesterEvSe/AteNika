// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include <cstddef>
#include <cstdint>

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
  // Upper half bits of the Zobrist key. Since lower bits already chose current slot,
  // This bits are stored to confirm that everything is ok and we chose proper slot.
  // Something can go wrong with probability 2^-32.
  uint32_t key;
  Move move;
  int32_t score;
  int16_t depth;
  uint8_t gen; // search generation: an entry from an older search loses its slot
  TTFlag flag;
};

// static_assert, because changing this layout changes the number of slots in the
// table, and so changes bench.
static_assert(sizeof(TTEntry) == 20, "TTEntry layout changed: bench will differ across platforms");

namespace TTable {
  constexpr size_t DEFAULT_HASH_MB = 16;
  constexpr size_t MIN_HASH_MB = 1;
  constexpr size_t MAX_HASH_MB = 1024;

  // Rounds down to a power-of-two entry count.
  void resize(size_t mb);
  [[nodiscard]] size_t size_mb();

  // For the bench command, which clears the table between positions.
  // Also used by the UCI "ucinewgame" command.
  void clear();

  // Bumped once per search, so a slot from an older search can be overwritten by
  // a newer result however deep the old one was.
  void new_search();

  // nullptr when the slot is empty or holds a different position.
  // The pointer stays valid until the next resize or clear.
  [[nodiscard]] const TTEntry *probe(const ZobristHash &zob);
  void add(const ZobristHash &zob, Move move, int32_t score, int16_t depth, TTFlag flag);
} // namespace TTable
