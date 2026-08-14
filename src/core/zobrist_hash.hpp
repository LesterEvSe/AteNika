// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include <cstdint>
#include <random>

#include "defs.hpp"

class Board;

class ZobristHash {
private:
  // Pseudo-Random Number Generator
  static constexpr uint64_t PRNG{0xF00DBEEF};
  static std::mt19937_64 gen;
  static std::uniform_int_distribution<uint64_t> dist;

  // color, piece, cell
  static uint64_t PIECE_KEYS[COLOR_SIZE][PIECE_SIZE][64];
  static uint64_t EN_PASSANT_FILE[8];
  static uint64_t QS_CASTLE[COLOR_SIZE];
  static uint64_t KS_CASTLE[COLOR_SIZE];
  static uint64_t WHITE_MOVE;

  uint64_t m_hash;

public:
  static void init();
  ZobristHash() : m_hash{0} {};
  void set_hash(const Board &board);
  [[nodiscard]] uint64_t get_hash() const;
  friend bool operator==(const ZobristHash &left, const ZobristHash &right);
  void operator=(const uint64_t &hash);

  void xor_piece(Color col, PieceType piece, uint8_t cell);
  void xor_move();

  void xor_en_passant(uint8_t ep_cell);

  void xor_white_ks_castling();
  void xor_white_qs_castling();
  void xor_black_ks_castling();
  void xor_black_qs_castling();
};
