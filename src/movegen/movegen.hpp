// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#ifndef ATENIKA_MOVEGEN_HPP
#define ATENIKA_MOVEGEN_HPP

#include "core/board.hpp"
#include "core/move_list.hpp"

class Movegen {
private:
  const Board &m_board;
  MoveList m_moves;

  Color m_us;
  Color m_them;
  uint8_t m_king_cell;
  bool m_double_check;

  // Answer to the question: "Can this move can stop check?"
  bitboard m_check_mask;

  // Occupancy with our own king cleared, for testing where the king may step.
  bitboard m_king_occupancy;

  // Only the entries named by m_pinned are written or read.
  bitboard m_pinned;
  bitboard m_pin_ray[64];

  void init_masks();

  // Destinations open to the piece on `from`, check and pin applied.
  [[nodiscard]] bitboard allowed_from(uint8_t from) const;
  [[nodiscard]] bool target_ok(uint8_t from, uint8_t to) const;
  [[nodiscard]] bitboard king_targets(bitboard targets) const;

  // En passant is the one case pins and check masks do not decide, because two
  // pawns leave the rank at once.
  [[nodiscard]] bool is_legal_slow(const Move &move) const;

  void add_moves(uint8_t from, bitboard moves, PieceType piece);
  void add_attacks(uint8_t from, bitboard attacks, PieceType move_piece, Color defender);

  void gen_moves();

  void gen_white_moves();
  void gen_black_moves();

  void gen_white_pawn_moves_and_captures();
  void gen_black_pawn_moves_and_captures();

  void gen_pawn_promotion(uint8_t from, uint8_t to, Move::Flag flag = Move::QUIET,
                          PieceType captured_piece = NONE);
  void gen_white_pawn_moves();
  void gen_black_pawn_moves();

  void gen_white_left_pawn_captures();
  void gen_black_left_pawn_captures();
  void gen_white_right_pawn_captures();
  void gen_black_right_pawn_captures();

  void gen_white_king_moves();
  void gen_black_king_moves();

  void gen_white_knight_moves();
  void gen_black_knight_moves();

  void gen_white_rook_moves();
  void gen_black_rook_moves();

  void gen_white_bishop_moves();
  void gen_black_bishop_moves();

  void gen_white_queen_moves();
  void gen_black_queen_moves();

public:
  explicit Movegen(const Board &board);
  [[nodiscard]] MoveList &get_legal_moves();
};

#endif // ATENIKA_MOVEGEN_HPP
