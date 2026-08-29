// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include "search/see.hpp"

#include <algorithm>

#include "bitboard/attacks.hpp"
#include "bitboard/bitfunc.hpp"

namespace {
  // One entry per capture in the sequence. A square cannot be captured on more
  // times than there are pieces on the board.
  constexpr int16_t MAX_SWAPS = 34;

  bitboard _attackers_to(const Board &board, uint8_t cell, bitboard occupancy) {
    const bitboard queens = board.get_pieces(WHITE, QUEEN) | board.get_pieces(BLACK, QUEEN);
    const bitboard bishops = board.get_pieces(WHITE, BISHOP) | board.get_pieces(BLACK, BISHOP);
    const bitboard rooks = board.get_pieces(WHITE, ROOK) | board.get_pieces(BLACK, ROOK);

    // get_pawn_attacks(WHITE, cell) is where a white pawn on `cell` would strike,
    // which is exactly where a black pawn attacking `cell` has to stand.
    bitboard result = Attacks::get_pawn_attacks(WHITE, cell) & board.get_pieces(BLACK, PAWN);
    result |= Attacks::get_pawn_attacks(BLACK, cell) & board.get_pieces(WHITE, PAWN);
    result |= Attacks::get_knight_attacks(cell) &
              (board.get_pieces(WHITE, KNIGHT) | board.get_pieces(BLACK, KNIGHT));
    result |= Attacks::get_king_attacks(cell) &
              (board.get_pieces(WHITE, KING) | board.get_pieces(BLACK, KING));
    result |= Attacks::get_bishop_attacks(cell, occupancy) & (bishops | queens);
    result |= Attacks::get_rook_attacks(cell, occupancy) & (rooks | queens);
    return result & occupancy;
  }

  // PIECES is in ascending value order, so the first hit is the cheapest attacker.
  bitboard _least_valuable(const Board &board, bitboard attackers, Color side, PieceType &piece) {
    for (PieceType type : PIECES) {
      const bitboard subset = attackers & board.get_pieces(side, type);
      if (subset != ZERO) {
        piece = type;
        return ONE << lsb(subset);
      }
    }
    return ZERO;
  }
} // namespace

bool See::can_lose_material(const Move &move) {
  if (!move.is_capture())
    return false;
  if (move.get_flag() == Move::CAPTURE_PROMOTION)
    return false;
  return VALUE[move.get_captured_piece()] < VALUE[move.get_move_piece()];
}

int32_t See::see(const Board &board, const Move &move) {
  const uint8_t to = move.get_to_cell();
  const Color mover = board.get_curr_move();

  bitboard occupancy = board.get_all_pieces();
  bitboard from_bb = ONE << move.get_from_cell();

  int32_t gain[MAX_SWAPS];
  int16_t d = 0;

  if (move.get_flag() == Move::EN_PASSANT) {
    gain[0] = VALUE[PAWN];
    // The captured pawn is beside the destination, not on it, and removing it
    // is what can expose a rook down the rank.
    reset(occupancy, static_cast<uint8_t>(mover == WHITE ? to - 8 : to + 8));
  } else {
    gain[0] = move.is_capture() ? VALUE[move.get_captured_piece()] : 0;
  }

  // Whatever ends up standing on `to` is the next victim, so a promotion counts
  // as the promoted piece from here on.
  PieceType on_square = move.get_move_piece();
  if (move.get_promotion_piece() != NONE) {
    gain[0] += VALUE[move.get_promotion_piece()] - VALUE[PAWN];
    on_square = move.get_promotion_piece();
  }

  const bitboard queens = board.get_pieces(WHITE, QUEEN) | board.get_pieces(BLACK, QUEEN);
  const bitboard bishops =
      board.get_pieces(WHITE, BISHOP) | board.get_pieces(BLACK, BISHOP) | queens;
  const bitboard rooks = board.get_pieces(WHITE, ROOK) | board.get_pieces(BLACK, ROOK) | queens;

  Color side = board.get_opponent_move();
  bitboard attackers = _attackers_to(board, to, occupancy);

  while (d < MAX_SWAPS - 1) {
    ++d;
    gain[d] = VALUE[on_square] - gain[d - 1];

    occupancy &= ~from_bb;
    attackers &= ~from_bb;

    // Removing the piece can uncover a slider that was standing behind it.
    attackers |= (Attacks::get_bishop_attacks(to, occupancy) & bishops) |
                 (Attacks::get_rook_attacks(to, occupancy) & rooks);
    attackers &= occupancy;

    from_bb = _least_valuable(board, attackers, side, on_square);
    if (from_bb == ZERO)
      break;

    if (on_square == KING && (attackers & board.get_side_pieces(get_opposite_move(side))) != ZERO)
      break;

    side = get_opposite_move(side);
  }

  // Walk back down: at every step the side to move could have declined to
  // recapture, so it takes the better of continuing and stopping.
  while (--d > 0)
    gain[d - 1] = -std::max(-gain[d - 1], gain[d]);

  return gain[0];
}
