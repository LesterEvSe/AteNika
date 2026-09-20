// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "defs.hpp"

class Board;

namespace NNUE {
  constexpr int32_t HIDDEN = 1024;

  // One slot per ply the board can hold, checked against Board::MAX_MOVES.
  constexpr int32_t MAX_PLIES = 2048;

  // The hidden layer before activation: feature_biases plus one weight row per
  // piece on the board, from both perspectives.
  struct alignas(64) Accumulator {
    int16_t values[COLOR_SIZE][HIDDEN];
  };

  // The triple a feature index is built from.
  struct Feature {
    Color color;
    PieceType piece;
    uint8_t cell;
  };

  // What one move changes in the input layer.
  // Maximum two changes, for example capture or promotion moves.
  // Without default initializer for optimization.
  struct Delta {
    Feature added[2];
    Feature removed[2];
    uint8_t added_count;
    uint8_t removed_count;

    void add(Color color, PieceType piece, uint8_t cell) {
      added[added_count++] = {color, piece, cell};
    }
    void remove(Color color, PieceType piece, uint8_t cell) {
      removed[removed_count++] = {color, piece, cell};
    }
  };

  // One accumulator per ply on the board's move stack.
  // Filled lazily, so `make()` only records delta.
  class AccumulatorStack {
  private:
    struct Entry {
      Accumulator acc;
      Delta delta;
      bool computed;
    };

    std::unique_ptr<Entry[]> m_entries;
    int32_t m_top;

    void invalidate(int32_t top);

  public:
    AccumulatorStack();
    AccumulatorStack(const AccumulatorStack &other);
    AccumulatorStack &operator=(const AccumulatorStack &other);

    void push(const Delta &delta);
    void pop();

    // `board` has to be the position the top entry describes.
    [[nodiscard]] const Accumulator &top(const Board &board);
  };

  // Builds the accumulator from scratch. Unoptimised way for debug verification.
  void refresh(const Board &board, Accumulator &acc);

  // Replaces the active net. On failure the previous one stays live.
  bool load(const std::string &path);

  [[nodiscard]] int32_t evaluate(const Board &board);

  namespace detail {
    constexpr int32_t INPUT = 768;

    // Must match the trainer's quantisation, see trainer/src/main.rs.
    // Activation is SCReLU (Squared Clipped ReLU), matching the trainer.

    // Quantization, scales feature weights and biases,
    constexpr int32_t QA = 255;

    // Scales output weights.
    constexpr int32_t QB = 64;

    constexpr int32_t SCALE = 400;

    [[nodiscard]] uint16_t feature_index(Color perspective, Color color, PieceType piece,
                                         uint8_t cell);

    // Run default `forward_scalar` version or optimized AVX2.
    [[nodiscard]] int32_t forward(const Accumulator &acc, Color side_to_move);

    // Raw calculation of NN without any optimization.
    [[nodiscard]] int32_t forward_scalar(const Accumulator &acc, Color side_to_move);

    // Whether the active net's output weights keep the SIMD path exact.
    [[nodiscard]] bool net_fits_simd();
  } // namespace detail
} // namespace NNUE
