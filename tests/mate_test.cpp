// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include <gtest/gtest.h>

#include "bitboard/attacks.hpp"
#include "bitboard/rays.hpp"
#include "core/board.hpp"
#include "core/zobrist_hash.hpp"
#include "eval/eval.hpp"
#include "search/mvv_lva.hpp"
#include "search/search.hpp"

// Check mate for n ply
class MateTest : public testing::Test {
public:
  static void SetUpTestCase() {
    ZobristHash::init();
    Rays::init();
    Attacks::init();
    MvvLva::init();
    Eval::init();
    Search::init();
    Search::set_depth(16);
    Search::set_time(INF);
  }
};

TEST_F(MateTest, mate_in_3_vice_lesson_60) {
  // Arrange
  Board board = Board("2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0 0");

  // Act
  Search::iter_deep(board, false);

  // Assert
  ASSERT_EQ("WM3", Search::get_mate());
}


// The tests are taken from https://github.com/TerjeKir/EngineTests/tree/master/testfiles

// White Mate in 1
TEST_F(MateTest, white_mate_in_one_1) {
  Board board = Board("3k3B/7p/p1Q1p3/2n5/6P1/K3b3/PP5q/R7 w - - 0 0");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM1", Search::get_mate());
}

TEST_F(MateTest, white_mate_in_one_2) {
  Board board = Board("4bk2/ppp3p1/2np3p/2b5/2B2Bnq/2N5/PP4PP/4RR1K w - - 0 0");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM1", Search::get_mate());
}

TEST_F(MateTest, white_mate_in_one_3) {
  Board board = Board("4rkr1/1p1Rn1pp/p3p2B/4Qp2/8/8/PPq2PPP/3R2K1 w - - 0 0");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM1", Search::get_mate());
}

TEST_F(MateTest, white_mate_in_one_4) {
  Board board = Board("5rkr/ppp2p1p/8/3qp3/2pN4/8/PPPQ1PPP/4R1K1 w - - 0 0");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM1", Search::get_mate());
}

TEST_F(MateTest, white_mate_in_one_5) {
  Board board = Board("rk5r/p1q2ppp/Qp1B1n2/2p5/2P5/6P1/PP3PBP/4R1K1 w - - 0 0");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM1", Search::get_mate());
}


// Black Mate in 1
TEST_F(MateTest, black_mate_in_one_1) {
  Board board = Board("r3k1nr/p1p2p1p/2pP4/8/7q/7b/PPPP3P/RNBQ2KR b kq - 0 0");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM1", Search::get_mate());
}

TEST_F(MateTest, black_mate_in_one_2) {
  Board board = Board("r2r2k1/ppp2pp1/5q1p/4p3/4bn2/2PB2N1/P1PQ1P1P/R4RK1 b - - 0 0");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM1", Search::get_mate());
}

TEST_F(MateTest, black_mate_in_one_3) {
  Board board = Board("r3k3/bppbq2r/p2p3p/3Pp2n/P1N1Pp2/2P2P1P/1PB3PN/R2QR2K b q - 0 5");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM1", Search::get_mate());
}

TEST_F(MateTest, black_mate_in_one_4) {
  Board board = Board("r4k1N/2p3pp/p7/1pbPn3/6b1/1P1P3P/1PP2qPK/RNB4Q b - - 0 3");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM1", Search::get_mate());
}

TEST_F(MateTest, black_mate_in_one_5) {
  Board board = Board("r6r/pppk1ppp/8/2b5/2P5/2Nb1N2/PPnK1nPP/1RB2B1R b - - 0 7");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM1", Search::get_mate());
}


// White Mate in 3
TEST_F(MateTest, white_mate_in_three_1) {
  Board board = Board("1k3r2/4R1Q1/p2q1r2/8/2p1Bb2/5R2/pP5P/K7 w - - 0 0");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM3", Search::get_mate());
}

TEST_F(MateTest, white_mate_in_three_2) {
  Board board = Board("5Kbk/6pp/6P1/8/8/8/8/7R w - - 0 0");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM3", Search::get_mate());
}


// White Mate in 7
TEST_F(MateTest, white_mate_in_seven_1) {
  Board board = Board("2R2nk1/5pp1/4p3/p3P1pP/3PQ3/8/rq3P2/2R3K1 w - - 0 0");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM7", Search::get_mate());
}

TEST_F(MateTest, white_mate_in_seven_2) {
  Board board = Board("2R5/3r1pk1/p2N3p/1pK1PPp1/8/6P1/PP2b2P/8 w - - 0 0");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM7", Search::get_mate());
}

TEST_F(MateTest, white_mate_in_seven_3) {
  Board board = Board("2R5/6kp/p2R2p1/1p2p3/q3n3/6N1/P5PP/5K2 w - - 0 2");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM7", Search::get_mate());
}

TEST_F(MateTest, white_mate_in_seven_4) {
  Board board = Board("2R5/kpQ5/p4r2/6p1/q1p2p2/P3n3/KPP5/8 w - - 0 0");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM7", Search::get_mate());
}

TEST_F(MateTest, white_mate_in_seven_5) {
  Board board = Board("2Rr1qk1/5ppp/p2N4/P7/5Q2/8/1r4PP/5BK1 w - - 0 0");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM7", Search::get_mate());
}


// Black Mate in 7
TEST_F(MateTest, black_mate_in_seven_1) {
  Board board = Board("3Rr2k/pp4pb/2p4p/2P1n3/1P1Q3P/4r1q1/PB4B1/5RK1 b - - 0 3");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM7", Search::get_mate());
}

TEST_F(MateTest, black_mate_in_seven_2) {
  Board board = Board("3k2r1/3b2pR/p2pppN1/7Q/1pn1P3/4q3/PPP3B1/1K1R2r1 b - - 0 0");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM7", Search::get_mate());
}

TEST_F(MateTest, black_mate_in_seven_3) {
  Board board = Board("3k4/1pp3b1/4b2p/1p3qp1/3Pn3/2P1RN2/r5P1/1Q2R1K1 b - - 0 0");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM7", Search::get_mate());
}

TEST_F(MateTest, black_mate_in_seven_4) {
  Board board = Board("3k4/R7/5N2/1p2n3/6p1/P1N2bP1/1r6/5K2 b - - 0 0");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM7", Search::get_mate());
}

TEST_F(MateTest, black_mate_in_seven_5) {
  Board board = Board("3q3k/6pp/3P3n/8/5B2/8/3Q1PP1/6RK b - - 0 4");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM7", Search::get_mate());
}

// Positions below from PGNs in matches/results, mate distance verified with Stockfish 18.

TEST_F(MateTest, DISABLED_quiet_key_move_white_11_ply_1) {
  Board board = Board("r5kr/p1p1Qp2/2p2P2/q1P4p/6p1/6P1/PP5P/3R1R1K w - - 4 29");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM11", Search::get_mate());
}

TEST_F(MateTest, DISABLED_quiet_key_move_white_11_ply_2) {
  Board board = Board("1r2r1k1/1p3p2/p4Pp1/5P1p/1P1pB3/5Q1P/P2Bb1K1/8 w - - 1 38");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM11", Search::get_mate());
}

TEST_F(MateTest, DISABLED_quiet_key_move_white_11_ply_3) {
  Board board = Board("4rk2/1q1n1p2/p2p4/2pP1R2/2B1P2Q/1P5P/1P4P1/7K w - - 5 36");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM11", Search::get_mate());
}

TEST_F(MateTest, DISABLED_quiet_key_move_black_11_ply_1) {
  Board board = Board("8/2p5/p1p1P1RR/3b2p1/P2Pk3/2B2r2/1P5K/6r1 b - - 2 51");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM11", Search::get_mate());
}

TEST_F(MateTest, quiet_key_move_white_7_ply_1) {
  Board board = Board("7k/5R2/7p/6p1/4N3/8/1r4P1/6K1 w - - 0 41");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM7", Search::get_mate());
}

TEST_F(MateTest, quiet_key_move_black_7_ply_1) {
  Board board = Board("5k2/2p3pp/1pq5/4Pp2/8/P3P1PK/BP3P1P/1RBr4 b - - 4 27");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM7", Search::get_mate());
}

TEST_F(MateTest, quiet_key_move_white_5_ply_1) {
  Board board = Board("rk1nr3/ppR4R/7p/4p3/4N3/PP2P3/2P1KPP1/8 w - - 1 33");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM5", Search::get_mate());
}

// A mate delivered on the fiftieth move stands - the game ends before any draw
// can be claimed. Both of these were scored as quiet centipawn positions until
// the terminal test was moved ahead of the MAX_PLY return.
TEST_F(MateTest, mate_on_the_fiftieth_move_1) {
  Board board = Board("8/8/4p3/5p2/2b4N/4k2P/1Q2n1PK/3q4 b - - 99 94");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM1", Search::get_mate());
}

TEST_F(MateTest, mate_on_the_fiftieth_move_2) {
  Board board = Board("8/8/3qp3/5p2/2b4N/4k2P/1Q2n1P1/7K b - - 97 93");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM3", Search::get_mate());
}

TEST_F(MateTest, quiet_key_move_black_5_ply_1) {
  Board board = Board("2B5/8/8/1b4p1/1B4P1/1p1rk3/1P6/2K5 b - - 37 67");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM5", Search::get_mate());
}


// Mined from matches/results, mate distance confirmed with Stockfish 18, then
// AteNika re-tested at "go depth 20 nodes 30000000".
//
// Two causes are already identified and worth attacking before the rest:
//
//   B - iter_deep breaks on the first mate it finds, so a longer mate found at
//       a shallow depth is locked in and the deeper iteration that would find
//       the shorter line never runs. Mate-distance pruning is the real fix.
//
//   C - DISABLED_missed_mate_check_4 scores 0, a draw, with a halfmove clock of
//       0, so it is not the fifty-move rule. That points at is_repetition()
//       firing on the second occurrence, which is not yet a draw.

// B: a mate is found, but not the shortest one

// key move: capture; engine says "mate 4", plays f4g4, reaches depth 7
// best line: f4g4 h5g4 e3g5 g8h8 g5g7
TEST_F(MateTest, DISABLED_suboptimal_mate_capture_1) {
  Board board = Board("5rk1/prn1Rp2/3p1P2/1qpP3p/5RbP/1B2QN2/1PP2P2/6K1 w - - 3 34");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM5", Search::get_mate());
}

// key move: check; engine says "mate 4", plays d2h6, reaches depth 8
// best line: d2h6 h8g8 f5f6 b5a6 h6g7
TEST_F(MateTest, DISABLED_suboptimal_mate_check_2) {
  Board board = Board("r4r1k/1p3p2/1q1p2p1/pb1P1P1p/P1Pp4/1P4R1/2BQ2K1/R7 w - - 0 28");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM5", Search::get_mate());
}

// key move: check; engine says "mate 5", plays e4d3, reaches depth 9
// best line: g3g2 h2h3 e4d3 e7e8r g1h1
TEST_F(MateTest, DISABLED_suboptimal_mate_check_3) {
  Board board = Board("8/2p1P3/p1p3RR/3b2p1/P2Pk3/2B3r1/1P5K/6r1 b - - 0 52");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM5", Search::get_mate());
}

// key move: quiet; engine says "mate 7", plays f1f5, reaches depth 12
// best line: f1f5 a5d2 d1d2 h8h6 f5g5 h6g6 g5g6 a8e8 d2d8 e8d8 e5e7
TEST_F(MateTest, DISABLED_suboptimal_mate_quiet_4) {
  Board board = Board("r4k1r/p1p2p2/2p2P2/q1P1Q2p/6p1/6P1/PP5P/3R1R1K w - - 2 28");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM11", Search::get_mate());
}

// key move: quiet; engine says "mate 4", plays e4f3, reaches depth 6
// best line: e4f3 h2h4 g4g3 h4h5 f1b5
TEST_F(MateTest, DISABLED_suboptimal_mate_quiet_5) {
  Board board = Board("8/2B5/1p3R1p/pP6/4kPp1/P7/7P/3r1bK1 b - - 0 43");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM5", Search::get_mate());
}

// key move: quiet; engine says "mate 4", plays g7g5, reaches depth 6
// best line: g7h8 f4h5 h8h5 e1c1 h5h1
TEST_F(MateTest, DISABLED_suboptimal_mate_quiet_6) {
  Board board = Board("r3r1k1/6q1/p2b4/1PpP1pP1/4nN2/4Bb2/PPQ2P2/R3R1K1 b - - 2 29");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM5", Search::get_mate());
}

// C: no mate found at all

// key move: capture; engine says "cp 870", plays g4g3, reaches depth 20
// best line: g4g3 b5f5 e2b2 f5g5 f3g4 g5g4 g3g4 f1e1 c4c3 e1d1 g4f3 d1c1 f3e3 c1d1 e3d3 d1c1 b2a2
// c1b1 a2a3 b1c1 a3a1
TEST_F(MateTest, DISABLED_missed_mate_capture_1) {
  Board board = Board("8/8/8/1R3p2/2p3k1/P4bP1/1P2r3/5K2 b - - 6 47");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM21", Search::get_mate());
}

// key move: capture; engine says "cp 955", plays f1e1, reaches depth 20
// best line: f1e1 g2g3 e1g1 e8d7 e6f6 d7h3 g5g4 a7h7 g4h3 h7h6 f6g5 h6g6 g5g6 g3g4 g1g2 h2h3 e2g1
// h3h4 g2g4
TEST_F(MateTest, DISABLED_missed_mate_capture_2) {
  Board board = Board("4B3/R7/1P1pk2p/P1p1p1p1/2P1P3/4n3/4n1PK/4Br2 b - - 10 41");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM19", Search::get_mate());
}

// key move: capture; engine says "cp 685", plays d2d5, reaches depth 14
// best line: c3a5 b7c6 a5a6 c6c5 e3c5 d6d5 c5a7 b8a8 a6c6 a8a7 c6c7 a7a8 e6a6
TEST_F(MateTest, DISABLED_missed_mate_capture_3) {
  Board board = Board("1k1r1brq/1pp3p1/2PpR1P1/p6p/P6P/1PQ1B1p1/2PR2K1/8 w - - 3 38");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM13", Search::get_mate());
}

// key move: check; engine says "cp 745", plays d2e2, reaches depth 20
// best line: d2e2 e1f1 g4g3 b5f5 e2b2 f5g5 f3g4 g5g4 g3g4 f1e1 c4c3 e1d1 g4f3 d1c1 b2f2 c1b1 f3e3
// a3a4 e3d3 a4a5 c3c2 b1b2 f2f1 b2b3 c2c1q b3b4 c1c4 b4a3 f1a1 a3b2 c4c3
TEST_F(MateTest, DISABLED_missed_mate_check_4) {
  Board board = Board("8/8/8/1R3p2/2p3k1/P4bP1/1P1r4/4K3 b - - 4 46");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM31", Search::get_mate());
}

// key move: check; engine says "cp 670", plays e8e5, reaches depth 19
// best line: e8c6 a2d5 c6d5 g2h3 d1g1 c1d2 d5g2 h3h4 g2h2 h4g5 h2h3 d2b4 f8e8 f2f3 g1g3 g5f4 g7g5
TEST_F(MateTest, DISABLED_missed_mate_check_5) {
  Board board = Board("4qk2/2p3pp/1p6/4Pp2/8/P3P1P1/BP3PKP/1RBr4 b - - 2 26");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM17", Search::get_mate());
}

// key move: check; engine says "cp 580", plays f4e3, reaches depth 14
// best line: g5e5 e8d8 f4g5 f7f6 f1f6 g7f6 e5f6 d8e8 a1f1 g8g5 f6g5 a6c7 g5g6 e8d8 f1f8 c7e8 g6e8
TEST_F(MateTest, DISABLED_missed_mate_check_6) {
  Board board = Board("r1q1kbr1/p2p1pp1/npbP4/1Np3Q1/2P1PB2/P2B4/1P5P/R4RK1 w q - 1 22");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM17", Search::get_mate());
}

// key move: check; engine says "cp 0", plays e4d3, reaches depth 16
// best line: f8f3 h3h2 f3g3 g6g5 g3g5 h6h4 e4d3 h4h3 d3c2 h3f3 d5f3 h2h3 g1h1
TEST_F(MateTest, DISABLED_missed_mate_check_7) {
  Board board = Board("5r2/2p5/p1p1P1RR/3b2p1/P2Pk3/2B4K/1P6/6r1 b - - 0 50");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM13", Search::get_mate());
}

// key move: quiet; engine says "cp 1760", plays e5f4, reaches depth 18
// best line: e5f5 g7f7 f5g6 f7f3 g4f3 h4g3 d7e8 g3f2 h5h4 f2f1 a3a2 f1g1 g6g5 g1f1 e8b5 f1e1 f3f2
// e1d1 f2f1q
TEST_F(MateTest, DISABLED_missed_mate_quiet_8) {
  Board board = Board("8/3bP1R1/8/4k2p/6pK/r7/8/8 b - - 7 75");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM19", Search::get_mate());
}

// key move: quiet; engine says "cp 930", plays f3f7, reaches depth 14
// best line: f3f7 d8d6 e3e7 d3d2 g3h2 c3g3 h2g3 d6d3 g3g2 d3g3 g2g3 d2d1q f7f8 g8h7 e7g7
TEST_F(MateTest, DISABLED_missed_mate_quiet_9) {
  Board board = Board("3r2k1/R5p1/8/7P/8/2qpQRK1/8/8 w - - 9 53");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM15", Search::get_mate());
}

// key move: quiet; engine says "cp 352", plays d2h6, reaches depth 16
// best line: d2h6 h8g8 h6g5 g8h8 f6f7 d4e2 c3e2 b6b2 c1b2 d8g8 g5f6 g8g7 f6g7
TEST_F(MateTest, DISABLED_missed_mate_quiet_10) {
  Board board = Board("3r3k/pp5p/1q1p1R2/2p5/2PnP3/2N5/PP1QB1P1/2K4n w - - 7 27");
  Search::iter_deep(board, false);
  ASSERT_EQ("WM13", Search::get_mate());
}

// key move: quiet; engine says "cp 140", plays b5c6, reaches depth 13
// best line: e7h4 a3c5 d6c5 d1f1 b6g6 d2f3 h4h3 f3e1 g6g2 e1g2 h3g2
TEST_F(MateTest, DISABLED_missed_mate_quiet_11) {
  Board board = Board("4r1k1/2p1qpp1/pr1p3p/1b6/2P1Pn2/QB1n4/PP1N1PPP/R1BR2K1 b - - 0 24");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM11", Search::get_mate());
}

// key move: quiet; engine says "cp 185", plays g2e4, reaches depth 15
// best line: e2f3 c7h7 g4g3 h2g1 g3h3 e3e4 h3h1
TEST_F(MateTest, DISABLED_missed_mate_quiet_12) {
  Board board = Board("8/2R5/4p1p1/5p2/2pB1PrP/4P3/1P2k1bK/8 b - - 21 66");
  Search::iter_deep(board, false);
  ASSERT_EQ("BM7", Search::get_mate());
}
