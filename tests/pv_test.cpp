// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include <string>

#include <gtest/gtest.h>

#include "bitboard/attacks.hpp"
#include "bitboard/rays.hpp"
#include "core/board.hpp"
#include "core/move_list.hpp"
#include "core/zobrist_hash.hpp"
#include "eval/eval.hpp"
#include "movegen/movegen.hpp"
#include "search/mvv_lva.hpp"
#include "search/search.hpp"
#include "search/ttable.hpp"

class PvTest : public testing::Test {
public:
  static void SetUpTestCase() {
    ZobristHash::init(); // Required for search
    Rays::init();
    Attacks::init(); // Must be init after Rays
    MvvLva::init();
    Eval::init();
    Search::init();
  }

  void SetUp() override {
    Search::set_time(INF);
    Search::set_max_nodes(0);

    TTable::clear();
    Search::new_game();
  }

  static void expect_pv_is_legal(Board board) {
    const Move *pv = Search::get_pv();

    for (int16_t i = 0; i < Search::get_pv_length(); ++i) {
      MoveList legal = Movegen(board).get_legal_moves();

      bool found = false;
      for (uint8_t j = 0; j < legal.size() && !found; ++j)
        found = (legal[j] == pv[i]);

      ASSERT_TRUE(found) << "pv[" << i << "] = " << static_cast<std::string>(pv[i])
                         << " is not legal in " << board.get_fen();
      board.make(pv[i]);
    }
  }
};

// The three positions below are the project's existing stress positions, reused
// from bench.cpp and perft_test.cpp rather than invented here.

TEST_F(PvTest, pv_is_legal_from_startpos) {
  Board board = Board();
  Search::set_depth(7);

  Search::iter_deep(board, false);

  expect_pv_is_legal(board);
}

TEST_F(PvTest, pv_is_legal_in_kiwipete) {
  Board board = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
  Search::set_depth(6);

  Search::iter_deep(board, false);

  expect_pv_is_legal(board);
}

TEST_F(PvTest, pv_is_legal_in_pawn_endgame) {
  Board board = Board("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0");
  Search::set_depth(8);

  Search::iter_deep(board, false);

  expect_pv_is_legal(board);
}

TEST_F(PvTest, pv_is_not_empty) {
  Board board = Board();
  Search::set_depth(5);

  Search::iter_deep(board, false);

  ASSERT_GT(Search::get_pv_length(), 0);
}

TEST_F(PvTest, pv_head_is_the_reported_best_move) {
  Board board = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
  Search::set_depth(6);

  Search::iter_deep(board, false);
  const Move *best = Search::get_best_move();

  ASSERT_NE(best, nullptr);
  ASSERT_TRUE(*best == Search::get_pv()[0]);
}

// Quiescence deliberately does not extend the PV, and check extensions only make
// the tree deeper.
TEST_F(PvTest, pv_never_exceeds_the_nominal_depth) {
  Board board = Board();

  for (int16_t depth = 1; depth <= 6; ++depth) {
    Search::set_depth(depth);
    Search::iter_deep(board, false);

    ASSERT_LE(Search::get_pv_length(), depth) << "at depth " << depth;
    ASSERT_GT(Search::get_pv_length(), 0) << "at depth " << depth;
  }
}

// A forced mate ends the line: iter_deep breaks out as soon as it sees one, so
// the PV is the mating sequence and stops there. Position from mate_test.cpp.
TEST_F(PvTest, pv_of_a_mate_in_one_is_a_single_move) {
  Board board = Board("3k3B/7p/p1Q1p3/2n5/6P1/K3b3/PP5q/R7 w - - 0 0");
  Search::set_depth(8);

  Search::iter_deep(board, false);

  ASSERT_EQ(Search::get_mate(), "WM1");
  ASSERT_EQ(Search::get_pv_length(), 1);
  expect_pv_is_legal(board);
}
