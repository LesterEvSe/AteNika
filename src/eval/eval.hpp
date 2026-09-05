// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include "core/board.hpp"

namespace Eval {
  void init();
  [[nodiscard]] int32_t evaluate(const Board &board);
} // namespace Eval
