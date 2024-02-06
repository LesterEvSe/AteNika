#include <gtest/gtest.h>
#include "rays.hpp"
#include "board.hpp"
#include "movegen.hpp"
#include "movepicker.hpp"

class MovePickerFixture : public testing::Test
{
protected:
    static int64_t count_moves(const Board &board, int16_t depth) {
        MoveList move_list = Movegen(board).get_legal_moves();

        if (depth-- == 1)
            return move_list.size();

        MovePicker move_picker = MovePicker(&move_list);
        int64_t result = 0;

        while (move_picker.has_next()) {
            Move move = move_picker.get_next();
            Board temp = board;
            temp.make(move);

            int64_t moves = count_moves(temp, depth);
            result += moves;
        }
        return result;
    }

public:
    static void SetUpTestCase() {
        init_bits_pre_calculation();
        ZobristHash::init();
        Rays::init();
        Attacks::init();
    }
};

// The tests are taken from
// https://gist.github.com/peterellisjones/8c46c28141c162d1d8a0f0badbc9cff9?permalink_comment_id=3775288
TEST_F(MovePickerFixture, github_test_1) {
    Board board = Board("r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3");
    int64_t actual = count_moves(board, 1);
    ASSERT_EQ(8, actual);
}

TEST_F(MovePickerFixture, github_test_2) {
    Board board = Board("8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0");
    int64_t actual = count_moves(board, 1);
    ASSERT_EQ(8, actual);
}

TEST_F(MovePickerFixture, github_test_3) {
    Board board = Board("r1bqkbnr/pppppppp/n7/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 2");
    int64_t actual = count_moves(board, 1);
    ASSERT_EQ(19, actual);
}

TEST_F(MovePickerFixture, github_test_4) {
    Board board = Board("r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQkq - 3");
    int64_t actual = count_moves(board, 1);
    ASSERT_EQ(5, actual);
}

TEST_F(MovePickerFixture, github_test_5) {
    Board board = Board("2kr3r/p1ppqpb1/bn2Qnp1/3PN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQ - 3");
    int64_t actual = count_moves(board,1);
    ASSERT_EQ(44, actual);
}

TEST_F(MovePickerFixture, github_test_6) {
    Board board = Board("rnb2k1r/pp1Pbppp/2p5/q7/2B5/8/PPPQNnPP/RNB1K2R w KQ - 3");
    int64_t actual = count_moves(board, 1);
    ASSERT_EQ(39, actual);
}

TEST_F(MovePickerFixture, github_test_7) {
    Board board = Board("2r5/3pk3/8/2P5/8/2K5/8/8 w - - 5");
    int64_t actual = count_moves(board, 1);
    ASSERT_EQ(9, actual);
}

TEST_F(MovePickerFixture, github_test_8) {
    Board board = Board("3k4/3p4/8/K1P4r/8/8/8/8 b - - 0");
    int64_t actual = count_moves(board, 6);
    ASSERT_EQ(1'134'888, actual);
}

TEST_F(MovePickerFixture, github_test_9) {
    Board board = Board("8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0");
    int64_t actual = count_moves(board, 6);
    ASSERT_EQ(1'015'133, actual);
}

TEST_F(MovePickerFixture, github_test_10) {
    Board board = Board("8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0");
    int64_t actual = count_moves(board, 6);
    ASSERT_EQ(1'440'467, actual);
}

TEST_F(MovePickerFixture, github_test_11) {
    Board board = Board("5k2/8/8/8/8/8/8/4K2R w K - 0");
    int64_t actual = count_moves(board, 6);
    ASSERT_EQ(661'072, actual);
}

TEST_F(MovePickerFixture, github_test_12) {
    Board board = Board("3k4/8/8/8/8/8/8/R3K3 w Q - 0");
    int64_t actual = count_moves(board, 6);
    ASSERT_EQ(803'711, actual);
}

TEST_F(MovePickerFixture, github_test_13) {
    Board board = Board("r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0");
    int64_t actual = count_moves(board, 4);
    ASSERT_EQ(1'274'206, actual);
}

TEST_F(MovePickerFixture, github_test_14) {
    Board board = Board("r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0");
    int64_t actual = count_moves(board, 4);
    ASSERT_EQ(1'720'476, actual);
}

TEST_F(MovePickerFixture, github_test_15) {
    Board board = Board("2K2r2/4P3/8/8/8/8/8/3k4 w - - 0");
    int64_t actual = count_moves(board, 6);
    ASSERT_EQ(3'821'001, actual);
}

TEST_F(MovePickerFixture, github_test_16) {
    Board board = Board("8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0");
    int64_t actual = count_moves(board, 5);
    ASSERT_EQ(1'004'658, actual);
}

TEST_F(MovePickerFixture, github_test_17) {
    Board board = Board("4k3/1P6/8/8/8/8/K7/8 w - - 0");
    int64_t actual = count_moves(board, 6);
    ASSERT_EQ(217'342, actual);
}

TEST_F(MovePickerFixture, github_test_18) {
    Board board = Board("8/P1k5/K7/8/8/8/8/8 w - - 0");
    int64_t actual = count_moves(board, 6);
    ASSERT_EQ(92'683, actual);
}

TEST_F(MovePickerFixture, github_test_19) {
    Board board = Board("K1k5/8/P7/8/8/8/8/8 w - - 0");
    int64_t actual = count_moves(board, 6);
    ASSERT_EQ(2'217, actual);
}

TEST_F(MovePickerFixture, github_test_20) {
    Board board = Board("8/k1P5/8/1K6/8/8/8/8 w - - 0");
    int64_t actual = count_moves(board, 7);
    ASSERT_EQ(567'584, actual);
}

TEST_F(MovePickerFixture, github_test_21) {
    Board board = Board("8/8/2k5/5q2/5n2/8/5K2/8 b - - 0");
    int64_t actual = count_moves(board, 4);
    ASSERT_EQ(23'527, actual);
}