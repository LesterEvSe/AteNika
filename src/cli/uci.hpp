// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

// Uci - Universal Chess Interface
// information from https://chessprogramming.org/UCI
namespace Uci {
  // Owns the read loop, the board and the search thread. Commands that
  // are not part of the protocol live in extras.hpp and are reached only after
  // every UCI keyword has been tried.
  void start();
} // namespace Uci
