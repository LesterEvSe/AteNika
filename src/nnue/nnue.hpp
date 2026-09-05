// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include <cstdint>
#include <string>

#include "core/board.hpp"
#include "defs.hpp"

namespace NNUE {
  namespace detail {
    constexpr int INPUT = 768;
    constexpr int HIDDEN = 512;

    // Must match the trainer's quantisation, see nets/gen_placeholder.py.
    // Using CReLU (Clipped ReLU) quantization params for this.

    // Quantization, scales feature weights and biases,
    constexpr int32_t QA = 255;

    // Scales output weights.
    constexpr int32_t QB = 64;
    constexpr int32_t SCALE = 400;

    struct alignas(64) Accumulator {
      int16_t values[COLOR_SIZE][HIDDEN];
    };

    [[nodiscard]] uint16_t feature_index(Color perspective, Color color, PieceType piece,
                                         uint8_t cell);
    void refresh(const Board &board, Accumulator &acc);
    [[nodiscard]] int32_t forward(const Accumulator &acc, Color side_to_move);
  } // namespace detail

  // Replaces the active net. On failure the previous one stays live.
  bool load(const std::string &path);

  [[nodiscard]] int32_t evaluate(const Board &board);
} // namespace NNUE
