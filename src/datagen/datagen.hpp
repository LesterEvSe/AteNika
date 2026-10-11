// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include <cstdint>
#include <string>

// Training data in bullet's text format, one "<fen> | <score> | <result>" per
// line, score in centipawns and result 1.0 / 0.5 / 0.0, both from White's side.
namespace Datagen {

  // Self-play from randomised openings, every move searched to `nodes`.
  void generate(int64_t nodes, int64_t positions, uint64_t seed, const std::string &path);

  // Re-searches every position of `in` from an empty table, keeping its result.
  void relabel(int64_t nodes, const std::string &in, const std::string &out);

} // namespace Datagen
