// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include <string>

#include "defs.hpp"

class Board;

// Used only 8 bytes
class Move {
public:
  // About tables: https://chessprogramming.org/Encoding_Moves
  // clang-format off
  enum Flag : uint8_t {
    //                          promo capture special
    QUIET             = 0,  //   0      0       00
    LONG_PAWN_MOVE    = 1,  //   0      0       01
    KSIDE_CASTLING    = 2,  //   0      0       10
    QSIDE_CASTLING    = 3,  //   0      0       11
    CAPTURE           = 4,  //   0      1       00
    EN_PASSANT        = 5,  //   0      1       01
    PROMOTION         = 8,  //   1      0       00
    CAPTURE_PROMOTION = 12, //   1      1       00
    NULL_MOVE         = 16, //   sentinel, clear of both bits
  };
  // clang-format on

private:
  // Bit-fields directly in the class rather than wrapped in an anonymous
  // struct: C++ has anonymous unions, not anonymous structs, and MSVC warns
  // (C4201). Consecutive bit-fields pack into the same allocation unit either.
  uint8_t m_from : 6;
  uint8_t m_to : 6;

  Flag m_flag : 5;
  PieceType m_move_piece : 3;
  PieceType m_captured_piece : 3;
  PieceType m_promotion_piece : 3;

public:
  Move()
      : m_from(0), m_to(0), m_flag(NULL_MOVE), m_move_piece(NONE), m_captured_piece(NONE),
        m_promotion_piece(NONE) {}

  // example e4e5 g2g1q or something else
  explicit Move(Board &board, const std::string &move);

  Move(uint8_t from, uint8_t to, PieceType move_piece, Flag flag = QUIET,
       PieceType captured_piece = NONE, PieceType promotion_piece = NONE)
      : m_from(from), m_to(to), m_flag(flag), m_move_piece(move_piece),
        m_captured_piece(captured_piece), m_promotion_piece(promotion_piece) {}

  [[nodiscard]] bool is_capture() const;
  // capture, en passant, or promotion
  [[nodiscard]] bool is_tactical() const;
  [[nodiscard]] uint8_t get_from_cell() const;
  [[nodiscard]] uint8_t get_to_cell() const;
  [[nodiscard]] Move::Flag get_flag() const;

  [[nodiscard]] PieceType get_move_piece() const;
  [[nodiscard]] PieceType get_captured_piece() const;
  [[nodiscard]] PieceType get_promotion_piece() const;
  [[nodiscard]] static bool is_move(const std::string &move);

  friend bool operator==(const Move &left, const Move &right);
  explicit operator std::string() const;
};
