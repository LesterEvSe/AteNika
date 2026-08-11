// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include "search/ttable.hpp"

std::unordered_map<uint64_t, TTEntry> TTable::detail::_ttable;

void TTable::add(const ZobristHash &zob, TTEntry entry) { detail::_ttable[zob.get_hash()] = entry; }

bool TTable::in_table(const ZobristHash &zob) {
  return detail::_ttable.find(zob.get_hash()) != detail::_ttable.end();
}

const TTEntry &TTable::get(const ZobristHash &zob) { return detail::_ttable[zob.get_hash()]; }

void TTable::clear() { detail::_ttable.clear(); }