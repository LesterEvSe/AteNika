// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include "core/board.hpp"

// https://www.chessprogramming.org/Perft_Results
namespace Perft {

  [[nodiscard]] int64_t run(Board &board, int depth);

  // The format matches Stockfish's `go perft`, so the two can be diffed
  // directly to find which root move disagrees, then recursed into.
  int64_t divide(Board &board, int depth);

} // namespace Perft
