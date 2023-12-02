#include <gtest/gtest.h>
#include "rays.hpp"
#include "board.hpp"
#include "movegen.hpp"

// using Stockfish 16 to calculation the expected value
/**
 * position fen (fen_pos)
 * go perft (depth)
 */

// https://www.chessprogramming.org/Perft_Results for position
// Perft - PERFomance Test, move path enumeration
class PerftFixture : public testing::Test
{
protected:
    static int64_t perft(const Board &board, int depth) {
        MoveList move_list = Movegen(board).get_legal_moves();

        // In order not to constantly subtract 1,
        // when calling the function
        if (depth-- == 1)
            return move_list.size();

        int64_t legal_moves = 0;
        for (uint8_t i = 0; i < move_list.size(); ++i) {
            Board temp = board;
            temp.make(move_list[i]);

            int64_t moves = perft(temp, depth);
            legal_moves += moves;
        }
        return legal_moves;
    }

public:
    static void SetUpTestCase() {
        init_bits_pre_calculation();
        Rays::init();
        Attacks::init();
    }
};

TEST_F(PerftFixture, max_possible_moves_in_position_1) {
    // Arrange
    Board board = Board("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 w - - 0");

    // Act
    int64_t actual = perft(board, 1);

    // Assert
    ASSERT_EQ(218, actual);
}

TEST_F(PerftFixture, max_possible_moves_in_position_2) {
    Board board = Board("3Q4/1Q4Q1/4Q3/2Q4R/Q4Q2/3Q4/1Q4Rp/1K1BBNNk w - - 0");
    int64_t actual = perft(board, 1);
    ASSERT_EQ(218, actual);
}

TEST_F(PerftFixture, check_en_passant_for_black) {
    Board board = Board("rnbqkbnr/pppp1ppp/8/8/4pP2/8/PPPPP1PP/RNBQKBNR b KQkq f3 0");
    int64_t actual = perft(board, 1);
    ASSERT_EQ(31, actual);
}

TEST_F(PerftFixture, black_right_pawn_attack) {
    // trouble with g7 h6
    Board board = Board("rnbq1k1r/pp1Pbppp/2p4B/8/2B5/8/PPP1NnPP/RN1QK2R b KQ - 2");
    ASSERT_EQ(31, perft(board, 1));
}

TEST_F(PerftFixture, unnecessary_en_passant_in_rank_1) {
    Board board = Board("kr6/pp6/8/8/8/8/1p6/QR5K b - - 0");
    ASSERT_EQ(14, perft(board, 1));
}

// chessprogramming wiki tests: https://www.chessprogramming.org/Perft_Results
TEST_F(PerftFixture, initial_position_1) {
    Board board = Board();
    const uint8_t size = 4;

    int64_t actual[size];
    int64_t expect[] = {20, 400, 8'902, 197'281};

    for (uint8_t i = 0; i < size; ++i)
        actual[i] = perft(board, i+1);

    for (uint8_t i = 0; i < size; ++i)
        ASSERT_EQ(expect[i], actual[i]);
}

TEST_F(PerftFixture, trouble_position_2) {
    Board board = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0");
    const uint8_t size = 4;

    int64_t actual[size];
    int64_t expect[] = {48, 2'039, 97'862, 4'085'603};

    for (uint8_t i = 0; i < size; ++i)
        actual[i] = perft(board, i+1);

    for (uint8_t i = 0; i < size; ++i)
        ASSERT_EQ(expect[i], actual[i]);
}

TEST_F(PerftFixture, trouble_position_3) {
    Board board = Board("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0");
    const uint8_t size = 6;

    int64_t actual[size];
    int64_t expect[] = {14, 191, 2'812, 43'238, 674'624, 11'030'083};

    for (uint8_t i = 0; i < size; ++i)
        actual[i] = perft(board, i+1);

    for (uint8_t i = 0; i < size; ++i)
        ASSERT_EQ(expect[i], actual[i]);
}

TEST_F(PerftFixture, trouble_position_4) {
    Board board = Board("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0");
    const uint8_t size = 5;

    int64_t actual[size];
    int64_t expect[] = {6, 264, 9'467, 422'333, 15'833'292};

    for (uint8_t i = 0; i < size; ++i)
        actual[i] = perft(board, i+1);

    for (uint8_t i = 0; i < size; ++i)
        ASSERT_EQ(expect[i], actual[i]);
}

TEST_F(PerftFixture, mirrored_trouble_position_4) {
    Board board = Board("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0");
    const uint8_t size = 5;

    int64_t actual[size];
    int64_t expect[] = {6, 264, 9'467, 422'333, 15'833'292};

    for (uint8_t i = 0; i < size; ++i)
        actual[i] = perft(board, i+1);

    for (uint8_t i = 0; i < size; ++i)
        ASSERT_EQ(expect[i], actual[i]);
}

TEST_F(PerftFixture, trouble_position_5) {
    Board board = Board("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1");
    const uint8_t size = 5;

    int64_t actual[size];
    int64_t expect[] = {44, 1'486, 62'379, 2'103'487, 89'941'194};

    for (uint8_t i = 0; i < size; ++i)
        actual[i] = perft(board, i+1);

    for (uint8_t i = 0; i < size; ++i)
        ASSERT_EQ(expect[i], actual[i]);
}

TEST_F(PerftFixture, trouble_position_6) {
    Board board = Board("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0");
    const uint8_t size = 4;

    int64_t actual[size];
    int64_t expect[] = {46, 2'079, 89'890, 3'894'594};

    for (uint8_t i = 0; i < size; ++i)
        actual[i] = perft(board, i+1);

    for (uint8_t i = 0; i < size; ++i)
        ASSERT_EQ(expect[i], actual[i]);
}

// The tests are taken from
// https://gist.github.com/peterellisjones/8c46c28141c162d1d8a0f0badbc9cff9?permalink_comment_id=3775288
TEST_F(PerftFixture, github_test_1) {
    Board board = Board("r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3");
    int64_t actual = perft(board, 1);
    ASSERT_EQ(8, actual);
}

TEST_F(PerftFixture, github_test_2) {
    Board board = Board("8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0");
    int64_t actual = perft(board, 1);
    ASSERT_EQ(8, actual);
}

TEST_F(PerftFixture, github_test_3) {
    Board board = Board("r1bqkbnr/pppppppp/n7/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 2");
    int64_t actual = perft(board, 1);
    ASSERT_EQ(19, actual);
}

TEST_F(PerftFixture, github_test_4) {
    Board board = Board("r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQkq - 3");
    int64_t actual = perft(board, 1);
    ASSERT_EQ(5, actual);
}

TEST_F(PerftFixture, github_test_5) {
    Board board = Board("2kr3r/p1ppqpb1/bn2Qnp1/3PN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQ - 3");
    int64_t actual = perft(board,1);
    ASSERT_EQ(44, actual);
}

TEST_F(PerftFixture, github_test_6) {
    Board board = Board("rnb2k1r/pp1Pbppp/2p5/q7/2B5/8/PPPQNnPP/RNB1K2R w KQ - 3");
    int64_t actual = perft(board, 1);
    ASSERT_EQ(39, actual);
}

TEST_F(PerftFixture, github_test_7) {
    Board board = Board("2r5/3pk3/8/2P5/8/2K5/8/8 w - - 5");
    int64_t actual = perft(board, 1);
    ASSERT_EQ(9, actual);
}

TEST_F(PerftFixture, github_test_8) {
    Board board = Board("3k4/3p4/8/K1P4r/8/8/8/8 b - - 0");
    int64_t actual = perft(board, 6);
    ASSERT_EQ(1'134'888, actual);
}

TEST_F(PerftFixture, github_test_9) {
    Board board = Board("8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0");
    int64_t actual = perft(board, 6);
    ASSERT_EQ(1'015'133, actual);
}

TEST_F(PerftFixture, github_test_10) {
    Board board = Board("8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0");
    int64_t actual = perft(board, 6);
    ASSERT_EQ(1'440'467, actual);
}

TEST_F(PerftFixture, github_test_11) {
    Board board = Board("5k2/8/8/8/8/8/8/4K2R w K - 0");
    int64_t actual = perft(board, 6);
    ASSERT_EQ(661'072, actual);
}

TEST_F(PerftFixture, github_test_12) {
    Board board = Board("3k4/8/8/8/8/8/8/R3K3 w Q - 0");
    int64_t actual = perft(board, 6);
    ASSERT_EQ(803'711, actual);
}

TEST_F(PerftFixture, github_test_13) {
    Board board = Board("r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0");
    int64_t actual = perft(board, 4);
    ASSERT_EQ(1'274'206, actual);
}

TEST_F(PerftFixture, github_test_14) {
    Board board = Board("r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0");
    int64_t actual = perft(board, 4);
    ASSERT_EQ(1'720'476, actual);
}

TEST_F(PerftFixture, github_test_15) {
    Board board = Board("2K2r2/4P3/8/8/8/8/8/3k4 w - - 0");
    int64_t actual = perft(board, 6);
    ASSERT_EQ(3'821'001, actual);
}

TEST_F(PerftFixture, github_test_16) {
    Board board = Board("8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0");
    int64_t actual = perft(board, 5);
    ASSERT_EQ(1'004'658, actual);
}

TEST_F(PerftFixture, github_test_17) {
    Board board = Board("4k3/1P6/8/8/8/8/K7/8 w - - 0");
    int64_t actual = perft(board, 6);
    ASSERT_EQ(217'342, actual);
}

TEST_F(PerftFixture, github_test_18) {
    Board board = Board("8/P1k5/K7/8/8/8/8/8 w - - 0");
    int64_t actual = perft(board, 6);
    ASSERT_EQ(92'683, actual);
}

TEST_F(PerftFixture, github_test_19) {
    Board board = Board("K1k5/8/P7/8/8/8/8/8 w - - 0");
    int64_t actual = perft(board, 6);
    ASSERT_EQ(2'217, actual);
}

TEST_F(PerftFixture, github_test_20) {
    Board board = Board("8/k1P5/8/1K6/8/8/8/8 w - - 0");
    int64_t actual = perft(board, 7);
    ASSERT_EQ(567'584, actual);
}

TEST_F(PerftFixture, github_test_21) {
    Board board = Board("8/8/2k5/5q2/5n2/8/5K2/8 b - - 0");
    int64_t actual = perft(board, 4);
    ASSERT_EQ(23'527, actual);
}