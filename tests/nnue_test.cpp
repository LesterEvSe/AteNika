// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include <format>
#include <set>
#include <string>

#include <gtest/gtest.h>

#include "bitboard/attacks.hpp"
#include "bitboard/rays.hpp"
#include "core/board.hpp"
#include "core/zobrist_hash.hpp"
#include "fen_mirror.hpp"
#include "nnue/nnue.hpp"

class NnueTest : public testing::Test {
public:
  static void SetUpTestCase() {
    ZobristHash::init();
    Rays::init();
    Attacks::init();
  }
};

TEST_F(NnueTest, feature_index_is_a_bijection) {
  for (const Color perspective : {BLACK, WHITE}) {
    std::set<uint16_t> seen;

    for (const Color color : {BLACK, WHITE})
      for (const PieceType piece : PIECES)
        for (uint8_t cell = 0; cell < 64; ++cell) {
          const uint16_t index = NNUE::detail::feature_index(perspective, color, piece, cell);
          ASSERT_LT(index, NNUE::detail::INPUT);
          ASSERT_TRUE(seen.insert(index).second) << "duplicate index " << index;
        }

    ASSERT_EQ(NNUE::detail::INPUT, seen.size());
  }
}

TEST_F(NnueTest, the_two_perspectives_are_mirrors) {
  for (const Color color : {BLACK, WHITE})
    for (const PieceType piece : PIECES)
      for (uint8_t cell = 0; cell < 64; ++cell) {
        const Color other = color == WHITE ? BLACK : WHITE;
        ASSERT_EQ(NNUE::detail::feature_index(WHITE, color, piece, cell),
                  NNUE::detail::feature_index(BLACK, other, piece, cell ^ 56));
      }
}

TEST_F(NnueTest, evaluation_is_mirror_symmetric) {
  for (const std::string &fen : FenMirror::POSITIONS) {
    const std::string mirrored = FenMirror::mirror(fen);
    SCOPED_TRACE(std::format("{}  mirrors to  {}", fen, mirrored));
    ASSERT_EQ(NNUE::evaluate(Board(fen)), NNUE::evaluate(Board(mirrored)));
  }
}

TEST_F(NnueTest, evaluation_is_deterministic) {
  for (const std::string &fen : FenMirror::POSITIONS) {
    SCOPED_TRACE(fen);
    const Board board(fen);
    ASSERT_EQ(NNUE::evaluate(board), NNUE::evaluate(board));
  }
}

TEST_F(NnueTest, the_embedded_net_is_live_without_any_init_call) {
  ASSERT_NE(0, NNUE::evaluate(Board(FenMirror::POSITIONS.front())));
}

TEST_F(NnueTest, load_rejects_a_file_of_the_wrong_size) {
  ASSERT_FALSE(NNUE::load("definitely-not-a-net.nnue"));
  ASSERT_FALSE(NNUE::load("README.md"));
}
