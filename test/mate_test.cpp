#include <gtest/gtest.h>
#include "rays.hpp"
#include "board.hpp"
#include "search.hpp"

// Check mate for n ply
class MateFixture : public testing::Test
{
protected:
    History history;

public:
    static void SetUpTestCase() {
        init_bits_pre_calculation();
        ZobristHash::init();
        Rays::init();
        PieceTables::init();
        Attacks::init();
        Search::init();
        Search::set_depth(16);
        Search::set_time(INF);
    }
};

// Templates for test
/*
TEST_F(MateFixture, white_mate_in__) {
    Board board = Board("");
    Search::iter_deep(board, false);
    ASSERT_EQ("WM", Search::get_mate());
}
*/

/*
TEST_F(MateFixture, black_mate_in__) {
    Board board = Board("");
    Search::iter_deep(board, false);
    ASSERT_EQ("BM", Search::get_mate());
}
*/

TEST_F(MateFixture, mate_in_3_vice_lesson_60) {
    // Arrange
    Board board = Board("2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0");

    // Act
    Search::iter_deep(history, board, false);

    // Assert
    ASSERT_EQ("WM3", Search::get_mate());
}


// The tests are taken from https://github.com/TerjeKir/EngineTests/tree/master/testfiles

// White Mate in 1
TEST_F(MateFixture, white_mate_in_one_1) {
    Board board = Board("3k3B/7p/p1Q1p3/2n5/6P1/K3b3/PP5q/R7 w - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("WM1", Search::get_mate());
}

TEST_F(MateFixture, white_mate_in_one_2) {
    Board board = Board("4bk2/ppp3p1/2np3p/2b5/2B2Bnq/2N5/PP4PP/4RR1K w - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("WM1", Search::get_mate());
}

TEST_F(MateFixture, white_mate_in_one_3) {
    Board board = Board("4rkr1/1p1Rn1pp/p3p2B/4Qp2/8/8/PPq2PPP/3R2K1 w - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("WM1", Search::get_mate());
}

TEST_F(MateFixture, white_mate_in_one_4) {
    Board board = Board("5rkr/ppp2p1p/8/3qp3/2pN4/8/PPPQ1PPP/4R1K1 w - - 0");
    Search::iter_deep(history,board, false);
    ASSERT_EQ("WM1", Search::get_mate());
}

TEST_F(MateFixture, white_mate_in_one_5) {
    Board board = Board("rk5r/p1q2ppp/Qp1B1n2/2p5/2P5/6P1/PP3PBP/4R1K1 w - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("WM1", Search::get_mate());
}


// Black Mate in 1
TEST_F(MateFixture, black_mate_in_one_1) {
    Board board = Board("r3k1nr/p1p2p1p/2pP4/8/7q/7b/PPPP3P/RNBQ2KR b kq - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("BM1", Search::get_mate());
}

TEST_F(MateFixture, black_mate_in_one_2) {
    Board board = Board("r2r2k1/ppp2pp1/5q1p/4p3/4bn2/2PB2N1/P1PQ1P1P/R4RK1 b - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("BM1", Search::get_mate());
}

TEST_F(MateFixture, black_mate_in_one_3) {
    Board board = Board("r3k3/bppbq2r/p2p3p/3Pp2n/P1N1Pp2/2P2P1P/1PB3PN/R2QR2K b q - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("BM1", Search::get_mate());
}

TEST_F(MateFixture, black_mate_in_one_4) {
    Board board = Board("r4k1N/2p3pp/p7/1pbPn3/6b1/1P1P3P/1PP2qPK/RNB4Q b - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("BM1", Search::get_mate());
}

TEST_F(MateFixture, black_mate_in_one_5) {
    Board board = Board("r6r/pppk1ppp/8/2b5/2P5/2Nb1N2/PPnK1nPP/1RB2B1R b - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("BM1", Search::get_mate());
}


// White Mate in 3
TEST_F(MateFixture, white_mate_in_three_1) {
    Board board = Board("1k3r2/4R1Q1/p2q1r2/8/2p1Bb2/5R2/pP5P/K7 w - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("WM3", Search::get_mate());
}

TEST_F(MateFixture, white_mate_in_three_2) {
    Board board = Board("5Kbk/6pp/6P1/8/8/8/8/7R w - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("WM3", Search::get_mate());
}


// White Mate in 7
TEST_F(MateFixture, white_mate_in_seven_1) {
    Board board = Board("2R2nk1/5pp1/4p3/p3P1pP/3PQ3/8/rq3P2/2R3K1 w - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("WM7", Search::get_mate());
}

TEST_F(MateFixture, white_mate_in_seven_2) {
    Board board = Board("2R5/3r1pk1/p2N3p/1pK1PPp1/8/6P1/PP2b2P/8 w - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("WM7", Search::get_mate());
}

TEST_F(MateFixture, white_mate_in_seven_3) {
    Board board = Board("2R5/6kp/p2R2p1/1p2p3/q3n3/6N1/P5PP/5K2 w - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("WM7", Search::get_mate());
}

TEST_F(MateFixture, white_mate_in_seven_4) {
    Board board = Board("2R5/kpQ5/p4r2/6p1/q1p2p2/P3n3/KPP5/8 w - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("WM7", Search::get_mate());
}

TEST_F(MateFixture, white_mate_in_seven_5) {
    Board board = Board("2Rr1qk1/5ppp/p2N4/P7/5Q2/8/1r4PP/5BK1 w - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("WM7", Search::get_mate());
}


// Black Mate in 7
TEST_F(MateFixture, black_mate_in_seven_1) {
    Board board = Board("3Rr2k/pp4pb/2p4p/2P1n3/1P1Q3P/4r1q1/PB4B1/5RK1 b - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("BM7", Search::get_mate());
}

TEST_F(MateFixture, black_mate_in_seven_2) {
    Board board = Board("3k2r1/3b2pR/p2pppN1/7Q/1pn1P3/4q3/PPP3B1/1K1R2r1 b - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("BM7", Search::get_mate());
}

TEST_F(MateFixture, black_mate_in_seven_3) {
    Board board = Board("3k4/1pp3b1/4b2p/1p3qp1/3Pn3/2P1RN2/r5P1/1Q2R1K1 b - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("BM7", Search::get_mate());
}

TEST_F(MateFixture, black_mate_in_seven_4) {
    Board board = Board("3k4/R7/5N2/1p2n3/6p1/P1N2bP1/1r6/5K2 b - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("BM7", Search::get_mate());
}

TEST_F(MateFixture, black_mate_in_seven_5) {
    Board board = Board("3q3k/6pp/3P3n/8/5B2/8/3Q1PP1/6RK b - - 0");
    Search::iter_deep(history, board, false);
    ASSERT_EQ("BM7", Search::get_mate());
}

// White Mate in 15
/*
4r2b/p1pppN2/1p1P4/3B1p2/2Pk4/BK6/4P1N1/8 w - - bm Nf7-g5; ce +M8; pv Nf7-g5 Kd4-e5 Ba3-b2+ Ke5xd6 Ng5-f7+ Kd6-c5 Bb2-a3+ Kc5-d4 Kb3-c2 f5-f4 e2-e3+ f4xe3 Ng2-h4 e3-e2 Nh4-f5+;
4r2r/p3ppk1/3p1np1/q2P1PQ1/1pp1PN1P/5n2/PPP5/1KN1R1R1 w - - bm Nf4-e6+; ce +M8; pv Nf4-e6+ Kg7-g8 Qg5xg6+ f7xg6 Rg1xg6+ Kg8-h7 Ne6-g5+ Nf3xg5 h4xg5 Qa5xa2+ Nc1xa2 e7-e6 Re1-h1+ Nf6-h5 Rh1xh5+;
4r3/1pPnqk2/7P/2pP4/p1P2B2/2PB3Q/P7/1K6 w - - bm Qh3-f5+; ce +M8; pv Qh3-f5+ Kf7-g8 Qf5-g6+ Kg8-h8 d5-d6 Qe7-e1+ Bf4-c1 Qe1xc1+ Kb1xc1 Re8-e1+ Kc1-c2 Re1-c1+ Kc2xc1 b7-b6 Qg6-h7+;
4r3/2R2p2/1n5k/p3PR1p/r2P1N2/3K3P/6P1/8 w - - bm Rc7xf7; ce +M8; pv Rc7xf7 Ra4-a3+ Kd3-e4 Ra3-e3+ Ke4xe3 Nb6-d5+ Nf4xd5 Re8-f8 Rf5-f6+ Kh6-g5 Rf7-g7+ Kg5-h4 Nd5-f4 a5-a4 g2-g3+;
4rk2/1b4qp/6pN/pp6/2B2b2/P1n1B2Q/5PPP/4RK2 w - - bm Be3-c5+; ce +M8; pv Be3-c5+ Bf4-d6 Bc5xd6+ Re8-e7 Re1xe7 Bb7xg2+ Kf1xg2 Qg7xe7 Qh3-e6 Kf8-g7 Nh6-f5+ g6xf5 Bd6-e5+ Qe7-f6 Qe6xf6+;
4rrk1/p1pb1ppp/1p6/n1qPB3/8/2PBR3/5PPP/3QR1K1 w - - bm Bd3xh7+; ce +M8; pv Bd3xh7+ Kg8-h8 Qd1-h5 Bd7-g4 Be5xg7+ Kh8xg7 Qh5-g5+ Kg7xh7 Qg5-h4+ Bg4-h5 Qh4xh5+ Kh7-g7 Re3-g3+ Kg7-f6 Qh5-f3+;
5B2/6PP/4NppP/3Bk1Pq/PP2P3/K1p5/1pppp3/8 w - - bm h7-h8N; ce +M8; pv h7-h8N b2-b1N+ Ka3-b3 c2-c1N+ Kb3-c4 Nb1-a3+ Kc4-c5 Nc1-b3+ Kc5-b6 Na3-c4+ Kb6-c7 f6xg5 g7-g8N g5-g4 Nh8-f7+;
5B2/Kn3b1k/p5p1/2p5/6p1/8/2r2r2/6Q1 w - - bm Qg1-a1; ce +M8; pv Qg1-a1 Rc2-b2 Qa1-c1 Rb2-d2 Qc1-c3 Rd2-d4 Qc3-e3 Rd4-f4 Qe3-e5 Rf4-f6 Qe5-g5 g4-g3 Qg5-h6+ Kh7-g8 Qh6-g7+;
5Bn1/7n/1b2Np2/4pP2/2p1p1Bp/pp4kP/4P3/3N3K w - - bm Ne6-g7; ce +M8; pv Ne6-g7 Kg3-f4 e2-e3+ Bb6xe3 Ng7-e6+ Kf4-g3 Bf8-b4 c4-c3 Bb4xc3 Be3-f2 Bc3xe5+ f6xe5 Nd1-c3 e4-e3 Nc3-e2+;
5K1k/3p2p1/5pPp/5p2/2p1P3/2p4P/8/6N1 w - -
 */

// Black Mate in 15
/*
3r2k1/Q4p2/Bp2p1pp/3r4/1P1n3q/P7/1P3PPP/2R1R1K1 b - - bm Nd4-f3+; ce +M8; pv Nd4-f3+ Kg1-f1 Nf3xh2+ Kf1-g1 Nh2-f3+ Kg1-f1 Rd5-d2 Qa7xf7+ Kg8xf7 Rc1-c7+ Kf7-f8 Rc7-f7+ Kf8xf7 g2-g3 Qh4-h1+;
3r3k/1p5p/7p/p4B2/4pKP1/3q3P/Pb1N1P2/3R1R2 b - - bm Rd8-e8 Rd8-d5; ce +M8; pv Rd8-e8 g4-g5 Bb2-e5+ Kf4-g4 Qd3-e2+ Kg4-h4 h6xg5+ Kh4xg5 Re8-g8+ Bf5-g6 h7xg6 f2-f3 Qe2-g2+ Kg5-h6 Be5-f4+;
3rk3/p1p2pp1/2p5/3bP3/7Q/6P1/PPP1KP2/RN5q b - - bm Qh1-d1+; ce +M8; pv Qh1-d1+ Ke2-e3 Qd1-e1+ Ke3-d3 Bd5-e4+ Kd3-c4 Qe1-e2+ Kc4-b4 Rd8-b8+ Kb4-c5 Rb8-b5+ Kc5-d4 c6-c5+ Kd4-c3 Qe2xc2+;
3rq1k1/pQ4pN/8/5p2/1p6/P1b1BP2/4KP1n/RB6 b - - bm Rd8-d2+; ce +M8; pv Rd8-d2+ Ke2-e1 Rd2-a2+ Ke1-d1 Qe8-d8+ Qb7-d7 Qd8xd7+ Kd1-c1 Bc3-d2+ Kc1-d1 Bd2xe3+ Bb1-d3 Qd7xd3+ Kd1-e1 Nh2xf3+;
4r1k1/5bp1/1p6/1Rp4p/2Pp1QPq/3P1P2/2P1r1B1/5RK1 b - - bm Re2xg2+; ce +M8; pv Re2xg2+ Kg1xg2 Re8-e2+ Rf1-f2 Re2xf2+ Kg2-g1 Rf2-e2 Qf4-b8+ Kg8-h7 Qb8-g8+ Bf7xg8 Rb5-b1 Qh4-h2+ Kg1-f1 Qh2-f2+;
4r1k1/5pp1/p6p/3n4/Pp2r3/8/1qQB2PP/2NK1B1R b - - bm Re4-e1+; ce +M8; pv Re4-e1+ Bd2xe1 Nd5-e3+ Kd1-e2 Ne3xc2+ Ke2-f3 Nc2xe1+ Kf3-g4 Re8-e4+ Kg4-g3 Re4-e3+ Kg3-g4 Qb2-d4+ Kg4-h5 Re3-e5+;
4r1k1/8/2p5/2p1Rpq1/6P1/1PP4Q/P5K1/RN1r4 b - - bm Re8xe5; ce +M8; pv Re8xe5 Qh3-f3 Re5-e1 c3-c4 Re1-g1+ Kg2-f2 Rd1-e1 Qf3-g3 Re1-f1+ Kf2-e2 Rg1xg3 g4xf5 Qg5-e3+ Ke2xf1 Rg3-g1+;
4r2k/pp6/3p4/1P3P2/2Pp3p/P2PnK1b/3QN1rP/1R2R3 b - - bm Bh3-g4+; ce +M8; pv Bh3-g4+ Kf3-f4 Rg2-f2+ Kf4-g5 Re8-g8+ Kg5-f6 Rf2xf5+ Kf6-e6 Rg8-g6+ Ke6-e7 Rg6-g7+ Ke7-e8 Bg4-h5+ Ke8-d8 Rf5-f8+;
5N1k/1b4pp/1p3q2/p3n3/P1B1p1r1/1P6/1Q2RPPP/5RK1 b - - bm Ne5-f3+; ce +M8; pv Ne5-f3+ Kg1-h1 Rg4-h4 h2-h3 Qf6-d6 Qb2xg7+ Kh8xg7 Nf8-e6+ Kg7-h8 Ne6-f4 Rh4xh3+ g2xh3 Qd6xf4 h3-h4 Qf4-h2+;
5bk1/p4ppp/Qp6/4B3/1P6/Pq2P1P1/2rr1P1P/R4RK1 b - -
 */