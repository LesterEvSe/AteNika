// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include "eval/eval.hpp"

#include "bitboard/bitfunc.hpp"
#include "defs.hpp"
#include "nnue/nnue.hpp"

namespace {
  bool can_force_mate(const Board &board, Color color) {
    if (board.get_pieces(color, QUEEN) || board.get_pieces(color, ROOK) ||
        board.get_pieces(color, PAWN))
      return true;

    const bitboard bishops = board.get_pieces(color, BISHOP);
    if ((bishops & WHITE_SQUARES) && (bishops & BLACK_SQUARES))
      return true;

    const uint8_t knights = count_bits(board.get_pieces(color, KNIGHT));
    return (bishops && knights) || knights > 2;
  }

  bool material_draw(const Board &board) {
    return !can_force_mate(board, WHITE) && !can_force_mate(board, BLACK);
  }
} // namespace

int32_t Eval::evaluate(const Board &board) {
  if (material_draw(board))
    return 0;
  return NNUE::evaluate(board);
}
