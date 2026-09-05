// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include <format>
#include <string>

#include <gtest/gtest.h>

#include "bitboard/attacks.hpp"
#include "bitboard/rays.hpp"
#include "core/board.hpp"
#include "core/zobrist_hash.hpp"
#include "eval/eval.hpp"
#include "fen_mirror.hpp"

class EvalTest : public testing::Test {
public:
  static void SetUpTestCase() {
    ZobristHash::init();
    Rays::init();
    Attacks::init();
    Eval::init();
  }

  static void expect_mirror_symmetric(const std::string &fen) {
    const std::string mirrored = FenMirror::mirror(fen);
    SCOPED_TRACE(std::format("{}  mirrors to  {}", fen, mirrored));
    ASSERT_EQ(Eval::evaluate(Board(fen)), Eval::evaluate(Board(mirrored)));
  }

  static void expect_involution(const std::string &fen) {
    SCOPED_TRACE(fen);
    ASSERT_EQ(fen, FenMirror::mirror(FenMirror::mirror(fen)));
  }

  static void expect_dead_draw(const std::string &fen) {
    SCOPED_TRACE(fen);
    ASSERT_EQ(0, Eval::evaluate(Board(fen)));
  }

  static void expect_not_drawn(const std::string &fen) {
    SCOPED_TRACE(fen);
    ASSERT_NE(0, Eval::evaluate(Board(fen)));
  }
};

TEST_F(EvalTest, mirror_flips_ranks_and_colours) {
  ASSERT_EQ("6q1/8/4k3/8/8/4K3/8/8 b - - 0 1",
            FenMirror::mirror("8/8/4k3/8/8/4K3/8/6Q1 w - - 0 1"));
}

TEST_F(EvalTest, mirror_flips_castling_and_en_passant) {
  ASSERT_EQ("4k3/8/8/8/8/8/8/R3K3 b Q - 0 1", FenMirror::mirror("r3k3/8/8/8/8/8/8/4K3 w q - 0 1"));
  ASSERT_EQ("rnbqkbnr/pppp1ppp/8/8/3PpP2/8/PPP1P1PP/RNBQKBNR b KQkq f3 0 3",
            FenMirror::mirror("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3"));
}

TEST_F(EvalTest, mirror_is_an_involution) {
  expect_involution("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  expect_involution("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
  expect_involution("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
  expect_involution("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
}

TEST_F(EvalTest, start_position_is_balanced) {
  ASSERT_EQ(0, Eval::evaluate(Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")));
}

TEST_F(EvalTest, mirror_symmetry_dense_positions) {
  expect_mirror_symmetric("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
  expect_mirror_symmetric("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1");
  expect_mirror_symmetric("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
  expect_mirror_symmetric("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
  expect_mirror_symmetric("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
}

TEST_F(EvalTest, mirror_symmetry_pawn_structure) {
  expect_mirror_symmetric("8/2p3k1/8/1P6/8/8/6K1/8 w - - 0 1");
  expect_mirror_symmetric("8/1P4k1/6p1/8/8/2P5/6K1/8 b - - 0 1");
  expect_mirror_symmetric("4k3/8/8/8/8/8/P1P1P3/4K3 w - - 0 1");
}

TEST_F(EvalTest, mirror_symmetry_open_files) {
  expect_mirror_symmetric("4k3/8/8/8/8/8/1PP5/R3K3 w Q - 0 1");
  expect_mirror_symmetric("4k3/p7/8/8/8/8/1PP5/R3K3 w Q - 0 1");
  expect_mirror_symmetric("4k3/8/8/8/8/8/1PP5/Q3K3 w - - 0 1");
}

TEST_F(EvalTest, mirror_symmetry_material_asymmetry) {
  expect_mirror_symmetric("4k3/8/8/8/8/8/8/2B1KB2 w - - 0 1");
  expect_mirror_symmetric("8/8/4k3/8/8/4K3/8/6Q1 w - - 0 1");
  expect_mirror_symmetric("8/8/4k3/8/8/4K3/8/6Q1 b - - 0 1");
}

TEST_F(EvalTest, mate_impossible_is_a_draw) {
  expect_dead_draw("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
  expect_dead_draw("4k3/8/8/8/8/8/8/4KB2 w - - 0 1");
  expect_dead_draw("4k3/8/8/8/8/8/8/3NKN2 w - - 0 1");
  expect_dead_draw("1b2k3/8/8/8/8/8/8/2B1K3 w - - 0 1");
  expect_dead_draw("4k3/8/8/8/8/8/8/1B1BKB2 w - - 0 1");
  expect_dead_draw("1n2kn2/8/8/8/8/8/8/1B2KB2 w - - 0 1");
}

TEST_F(EvalTest, mate_possible_is_not_a_draw) {
  expect_not_drawn("4k3/8/8/8/8/8/8/2B1KB2 w - - 0 1");
  expect_not_drawn("4k3/8/8/8/8/8/8/3BKN2 w - - 0 1");
  expect_not_drawn("4k3/8/8/8/8/8/8/1N1NKN2 w - - 0 1");
  expect_not_drawn("4k3/8/8/8/8/8/8/R3K3 w - - 0 1");
}

TEST_F(EvalTest, drawish_endings_are_not_claimed) {
  expect_not_drawn("2b1k3/8/8/8/8/8/8/R3K3 w - - 0 1");
  expect_not_drawn("r3k3/8/8/8/8/8/8/R2BK3 w - - 0 1");
}
