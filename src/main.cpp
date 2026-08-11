// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include "bitboard/attacks.hpp"
#include "bitboard/rays.hpp"
#include "cli/uci.hpp"
#include "core/zobrist_hash.hpp"
#include "eval/eval.hpp"
#include "search/mvv_lva.hpp"
#include "search/search.hpp"

int main() {
  ZobristHash::init();
  Rays::init();
  Attacks::init(); // Must be init after Rays
  MvvLva::init();
  Eval::init();
  Search::init();
  Uci::start();
  return 0;
}
