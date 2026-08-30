// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include "core/zobrist_hash.hpp"

#include "bitboard/bitfunc.hpp"
#include "core/board.hpp"

std::mt19937_64 ZobristHash::gen{PRNG}; // NOLINT(bugprone-random-generator-seed)
std::uniform_int_distribution<uint64_t> ZobristHash::dist;

uint64_t ZobristHash::PIECE_KEYS[COLOR_SIZE][PIECE_SIZE][64];
uint64_t ZobristHash::EN_PASSANT_FILE[8];
uint64_t ZobristHash::QS_CASTLE[COLOR_SIZE];
uint64_t ZobristHash::KS_CASTLE[COLOR_SIZE];
uint64_t ZobristHash::WHITE_MOVE;

void ZobristHash::init() {
  // Reseeded, to fix bug with different hashes from the test to test.
  gen.seed(PRNG);

  for (uint8_t j = 0; j < PIECE_SIZE; ++j)
    for (uint8_t k = 0; k < 64; ++k) {
      PIECE_KEYS[BLACK][j][k] = dist(gen);
      PIECE_KEYS[WHITE][j][k] = dist(gen);
    }

  for (auto &i : EN_PASSANT_FILE)
    i = dist(gen);

  for (uint8_t i = 0; i < COLOR_SIZE; ++i) {
    QS_CASTLE[i] = dist(gen);
    KS_CASTLE[i] = dist(gen);
  }
  WHITE_MOVE = dist(gen);
}

void ZobristHash::set_hash(const Board &board) {
  if (board.get_curr_move() == WHITE)
    xor_move();

  // xor_en_passant takes a cell, not a file; it calls get_file itself.
  if (board.get_en_passant())
    xor_en_passant(board.get_en_passant());

  if (board.get_white_ks_castle())
    xor_white_ks_castling();
  if (board.get_white_qs_castle())
    xor_white_qs_castling();
  if (board.get_black_ks_castle())
    xor_black_ks_castling();
  if (board.get_black_qs_castle())
    xor_black_qs_castling();

  for (auto i : PIECES) {
    bitboard white_pieces = board.get_pieces(WHITE, i);
    bitboard black_pieces = board.get_pieces(BLACK, i);

    for (uint8_t j = 0; j < 64; ++j) {
      uint64_t cell = ONE << j;
      if (white_pieces & cell)
        xor_piece(WHITE, i, j);
      else if (black_pieces & cell)
        xor_piece(BLACK, i, j);
    }
  }
}

uint64_t ZobristHash::get_hash() const { return m_hash; }

void ZobristHash::operator=(const uint64_t &hash) { m_hash = hash; }

bool operator==(const ZobristHash &left, const ZobristHash &right) {
  return left.m_hash == right.m_hash;
}

void ZobristHash::xor_piece(Color col, PieceType piece, uint8_t cell) {
  m_hash ^= PIECE_KEYS[col][piece][cell];
}
void ZobristHash::xor_move() { m_hash ^= WHITE_MOVE; }
void ZobristHash::xor_en_passant(uint8_t ep_cell) { m_hash ^= EN_PASSANT_FILE[get_file(ep_cell)]; }
void ZobristHash::xor_white_ks_castling() { m_hash ^= KS_CASTLE[WHITE]; }
void ZobristHash::xor_white_qs_castling() { m_hash ^= QS_CASTLE[WHITE]; }
void ZobristHash::xor_black_ks_castling() { m_hash ^= KS_CASTLE[BLACK]; }
void ZobristHash::xor_black_qs_castling() { m_hash ^= QS_CASTLE[BLACK]; }
