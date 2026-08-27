// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include "core/board.hpp"
#include "core/move.hpp"
#include "defs.hpp"

// Static Exchange Evaluation. Plays out every capture on the destination square,
// cheapest attacker first, and returns the material the mover ends up with when
// both sides stop at their best moment.
//
// More here: https://www.chessprogramming.org/Static_Exchange_Evaluation
namespace See {
  constexpr int32_t VALUE[PIECE_SIZE]{100, 320, 330, 500, 900, 10'000};

  [[nodiscard]] int32_t see(const Board &board, const Move &move);

  // value(victim) >= value(attacker) implies see() >= 0, because the mover can
  // lose at most its own piece after winning the victim.
  [[nodiscard]] bool can_lose_material(const Move &move);
} // namespace See
