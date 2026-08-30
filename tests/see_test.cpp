// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include <gtest/gtest.h>

#include "bitboard/attacks.hpp"
#include "bitboard/rays.hpp"
#include "core/board.hpp"
#include "core/move.hpp"
#include "core/zobrist_hash.hpp"
#include "search/see.hpp" // You can find the piece values here.

class SeeTest : public testing::Test {
public:
  static void SetUpTestCase() {
    ZobristHash::init();
    Rays::init();
    Attacks::init();
  }

  static int32_t see(const std::string &fen, const std::string &move) {
    Board board = Board(fen);
    return See::see(board, Move(board, move));
  }
};

TEST_F(SeeTest, wins_an_undefended_pawn) {
  ASSERT_EQ(100, see("4k3/8/8/4p3/8/8/8/4R1K1 w - - 0 1", "e1e5"));
}

TEST_F(SeeTest, rook_takes_pawn_defended_by_pawn) {
  ASSERT_EQ(-400, see("4k3/8/5p2/4p3/8/8/8/4R1K1 w - - 0 1", "e1e5"));
}

TEST_F(SeeTest, wins_an_undefended_rook) {
  ASSERT_EQ(500, see("4k3/8/8/4r3/8/8/8/4R1K1 w - - 0 1", "e1e5"));
}

TEST_F(SeeTest, even_rook_trade_is_zero) {
  ASSERT_EQ(0, see("4k3/8/5p2/4r3/8/8/8/4R1K1 w - - 0 1", "e1e5"));
}

// The e1 rook only reaches e5 once the e2 rook has left it, so a SEE that does
// not re-scan for sliders after each capture answers -400 here.
TEST_F(SeeTest, doubled_rooks_see_through_each_other) {
  ASSERT_EQ(-300, see("4k3/8/5p2/4p3/8/8/4R3/4R1K1 w - - 0 1", "e2e5"));
}

TEST_F(SeeTest, queen_takes_defended_pawn) {
  ASSERT_EQ(-800, see("4k3/8/5p2/4p3/8/8/8/4Q1K1 w - - 0 1", "e1e5"));
}

TEST_F(SeeTest, knight_takes_defended_pawn) {
  ASSERT_EQ(-220, see("4k3/8/5p2/4p3/8/3N4/8/6K1 w - - 0 1", "d3e5"));
}

// dxe5 fxe5 Rxe5: two pawns won for one lost. Wrong without the rollback taking
// the better of capturing and standing still at every step.
TEST_F(SeeTest, supported_pawn_capture_wins_a_pawn) {
  ASSERT_EQ(100, see("4k3/8/5p2/4p3/3P4/8/8/4R1K1 w - - 0 1", "d4e5"));
}

TEST_F(SeeTest, black_to_move_is_scored_for_black) {
  ASSERT_EQ(100, see("4k3/8/8/4r3/4P3/8/8/4K3 b - - 0 1", "e5e4"));
}

TEST_F(SeeTest, quiet_move_into_an_attacked_square_loses_the_piece) {
  ASSERT_EQ(-500, see("4k3/8/5p2/8/8/8/8/4R1K1 w - - 0 1", "e1e5"));
}
