// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include "search/ttable.hpp"

#include <algorithm>
#include <bit>
#include <vector>

namespace {
  // Returns 1 or more, so the table is never empty and indexing is always defined.
  constexpr size_t _entries_for(size_t mb) {
    const size_t count = std::bit_floor(mb * 1024 * 1024 / sizeof(TTEntry));
    return count > 0 ? count : 1;
  }
} // namespace

// Filled at static-initialisation time, so no caller has to remember to call an
// init function before the first probe.
namespace TTable::detail {
  std::vector<TTEntry> _table(_entries_for(DEFAULT_HASH_MB));
  size_t _mask = _table.size() - 1;
  size_t _mb = DEFAULT_HASH_MB;
  uint8_t _gen = 0;
} // namespace TTable::detail

void TTable::resize(size_t mb) {
  mb = std::clamp(mb, MIN_HASH_MB, MAX_HASH_MB);

  detail::_mb = mb;
  detail::_table.assign(_entries_for(mb), TTEntry{});
  detail::_mask = detail::_table.size() - 1;
  detail::_gen = 0;
}

size_t TTable::size_mb() { return detail::_mb; }

void TTable::clear() {
  std::fill(detail::_table.begin(), detail::_table.end(), TTEntry{});
  detail::_gen = 0;
}

void TTable::new_search() { ++detail::_gen; }

const TTEntry *TTable::probe(const ZobristHash &zob) {
  const uint64_t key = zob.get_hash();
  const TTEntry &entry = detail::_table[key & detail::_mask];

  if (entry.flag == TTFlag::NONE || entry.key != static_cast<uint32_t>(key >> 32))
    return nullptr;
  return &entry;
}

void TTable::add(const ZobristHash &zob, Move move, int32_t score, int16_t depth, TTFlag flag) {
  const uint64_t key = zob.get_hash();
  TTEntry &slot = detail::_table[key & detail::_mask];

  // _gen in action
  const bool occupied = slot.flag != TTFlag::NONE && slot.gen == detail::_gen;
  if (occupied && depth < slot.depth)
    return;

  slot = TTEntry{static_cast<uint32_t>(key >> 32), move, score, depth, detail::_gen, flag};
}
