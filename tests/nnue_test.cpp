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

TEST_F(NnueTest, the_simd_output_layer_matches_the_scalar_one) {
  uint64_t state = 0x9E3779B97F4A7C15;
  const auto next_value = [&state] {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    return static_cast<int16_t>(static_cast<int32_t>((state >> 33) % 4001) - 2000);
  };

  NNUE::Accumulator acc{};

  for (int trial = 0; trial < 2000; ++trial) {
    for (const Color perspective : {BLACK, WHITE})
      for (int i = 0; i < NNUE::HIDDEN; ++i)
        acc.values[perspective][i] = next_value();

    for (const Color stm : {BLACK, WHITE})
      ASSERT_EQ(NNUE::detail::forward_scalar(acc, stm), NNUE::detail::forward(acc, stm))
          << "trial " << trial;
  }
}

TEST_F(NnueTest, the_embedded_net_keeps_the_simd_path_exact) {
  ASSERT_TRUE(NNUE::detail::net_fits_simd());
}

TEST_F(NnueTest, load_rejects_a_file_of_the_wrong_size) {
  ASSERT_FALSE(NNUE::load("definitely-not-a-net.nnue"));
  ASSERT_FALSE(NNUE::load("README.md"));
}

namespace {
  int32_t refreshed(const Board &board) { return NNUE::evaluate(Board(board.get_fen())); }
} // namespace

TEST_F(NnueTest, every_move_kind_updates_the_accumulator) {
  // clang-format off
  const struct { const char *fen, *move; } CASES[] = {
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",   "g1f3"}, // quiet
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",   "e2e4"}, // long pawn move
    {"rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1", "e4d5"}, // capture
    {"8/8/8/3pP3/8/8/8/K6k w - d6 0 1",                            "e5d6"}, // en passant
    {"7k/P7/8/8/8/8/8/K7 w - - 0 1",                              "a7a8q"}, // promotion
    {"1n5k/P7/8/8/8/8/8/K7 w - - 0 1",                            "a7b8q"}, // capture promotion
    {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1",                       "e1g1"}, // white kingside
    {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1",                       "e1c1"}, // white queenside
    {"r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1",                       "e8g8"}, // black kingside
    {"r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1",                       "e8c8"}, // black queenside
    {"8/8/8/8/3Pp3/8/8/K6k b - d3 0 1",                            "e4d3"}, // en passant, black
    {"k7/8/8/8/8/8/p7/7K b - - 0 1",                              "a2a1q"}, // promotion, black
    {"k7/8/8/8/8/8/p7/1N5K b - - 0 1",                            "a2b1q"}, // capture promotion, black
    {"7k/P7/8/8/8/8/8/K7 w - - 0 1",                              "a7a8n"}, // under-promotion
  };
  // clang-format on

  for (const auto &[fen, notation] : CASES) {
    SCOPED_TRACE(std::format("{} after {}", fen, notation));

    Board board(fen);
    const Move move(board, notation);
    const int32_t before = NNUE::evaluate(board);

    board.make(move);
    ASSERT_EQ(refreshed(board), NNUE::evaluate(board));

    board.unmake(move);
    ASSERT_EQ(before, NNUE::evaluate(board));
  }
}

TEST_F(NnueTest, a_chain_of_unevaluated_moves_lands_on_the_refreshed_value) {
  const char *GAME[] = {"e2e4", "c7c5", "g1f3", "d7d6", "d2d4", "c5d4", "f3d4", "g8f6", "b1c3",
                        "a7a6", "f1e2", "e7e5", "d4b3", "f8e7", "e1g1", "e8g8", "c1e3", "c8e6"};

  Board board;
  for (const char *notation : GAME)
    board.make(Move(board, notation));

  ASSERT_EQ(refreshed(board), NNUE::evaluate(board));
}

TEST_F(NnueTest, a_null_move_needs_no_accumulator_entry) {
  for (const std::string &fen : FenMirror::POSITIONS) {
    SCOPED_TRACE(fen);

    Board board(fen);

    const int32_t before = NNUE::evaluate(board);

    board.make_null_move();
    ASSERT_EQ(refreshed(board), NNUE::evaluate(board));

    board.unmake_null_move();
    ASSERT_EQ(before, NNUE::evaluate(board));
  }
}
