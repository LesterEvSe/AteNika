// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include "nnue/nnue.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iterator>
#include <vector>

#include "bitboard/bitfunc.hpp"

// Defined by the file CMake generates from ATENIKA_NET.
extern const unsigned char ATENIKA_NET[];

namespace {
  using namespace NNUE::detail;

  // alignas(64) mirrors bullet's `#[repr(C, align(64))]` accumulator.
  struct alignas(64) Network {
    int16_t feature_weights[INPUT][HIDDEN];
    int16_t feature_biases[HIDDEN];
    int16_t output_weights[2 * HIDDEN];
    int16_t output_bias;
  };

  static_assert(ATENIKA_NET_SIZE == sizeof(Network),
                "the embedded net does not match the layout above");

  Network &net() {
    static Network instance = [] {
      Network fresh;
      std::memcpy(&fresh, ATENIKA_NET, sizeof(fresh));
      return fresh;
    }();

    return instance;
  }

} // namespace

bool NNUE::load(const std::string &path) {
  std::ifstream in(path, std::ios::binary);
  if (!in)
    return false;

  const std::vector<char> buffer((std::istreambuf_iterator<char>(in)),
                                 std::istreambuf_iterator<char>());
  if (buffer.size() != sizeof(Network))
    return false;

  std::memcpy(&net(), buffer.data(), sizeof(Network));
  return true;
}

uint16_t NNUE::detail::feature_index(Color perspective, Color color, PieceType piece,
                                     uint8_t cell) {
  const uint16_t relative_color = color == perspective ? 0 : 1;
  const uint8_t relative_cell = perspective == WHITE ? cell : cell ^ 56;
  return relative_color * 384 + static_cast<uint16_t>(piece) * 64 + relative_cell;
}

void NNUE::detail::refresh(const Board &board, Accumulator &acc) {
  for (const Color perspective : {BLACK, WHITE})
    std::copy_n(net().feature_biases, HIDDEN, acc.values[perspective]);

  for (const Color color : {BLACK, WHITE})
    for (const PieceType piece : PIECES) {
      bitboard pieces = board.get_pieces(color, piece);

      while (pieces) {
        const uint8_t cell = pop_lsb(pieces);

        for (const Color perspective : {BLACK, WHITE}) {
          const int16_t *column =
              net().feature_weights[feature_index(perspective, color, piece, cell)];

          for (int i = 0; i < HIDDEN; ++i)
            acc.values[perspective][i] =
                static_cast<int16_t>(acc.values[perspective][i] + column[i]);
        }
      }
    }
}

int32_t NNUE::detail::forward(const Accumulator &acc, Color side_to_move) {
  const Color them = side_to_move == WHITE ? BLACK : WHITE;

  // int64 to prevent overflow.
  int64_t sum = 0;

  for (int i = 0; i < HIDDEN; ++i) {
    const int64_t us = std::clamp<int32_t>(acc.values[side_to_move][i], 0, QA);
    const int64_t opponent = std::clamp<int32_t>(acc.values[them][i], 0, QA);

    sum += us * us * net().output_weights[i];
    sum += opponent * opponent * net().output_weights[HIDDEN + i];
  }

  sum /= QA;
  sum += net().output_bias;
  return static_cast<int32_t>(sum * SCALE / (QA * QB));
}

int32_t NNUE::evaluate(const Board &board) {
  detail::Accumulator acc{};
  detail::refresh(board, acc);
  return detail::forward(acc, board.get_curr_move());
}
