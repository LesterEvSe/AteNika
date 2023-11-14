#include <gtest/gtest.h>
#include "rays.hpp"
#include "eval.hpp"

using namespace Eval;
using namespace hidden;

class EvalTestFixture : public testing::Test
{
public:
    static void SetUpTestCase() {
        Rays::init();
        Attacks::init();
        PieceTables::init();
        Eval::init();
    }
};

// It is hard enough testing only evaluation function,
// that's why I am testing hidden functions

// Doubled rooks test
TEST_F(EvalTestFixture, doubled_rooks_empty_field) {
    Board board = Board("8/8/8/8/8/R7/8/4R3");

    // Act
    bool doubled = _doubled_rooks(board, WHITE);

    // Assert
    ASSERT_FALSE(doubled);
}

TEST_F(EvalTestFixture, doubled_rooks_empty_field_true_file) {
    Board board = Board("8/8/8/8/8/4R3/8/4R3");
    ASSERT_TRUE(_doubled_rooks(board, WHITE));
}

TEST_F(EvalTestFixture, doubled_rooks_empty_field_true_rank) {
    Board board = Board("8/8/8/1R3R2/8/8/8/8");
    ASSERT_TRUE(_doubled_rooks(board, WHITE));
}

TEST_F(EvalTestFixture, doubled_rooks_with_blockers) {
    Board board = Board("8/8/8/1R1p1R2/8/8/8/8");
    ASSERT_FALSE(_doubled_rooks(board, WHITE));
}

TEST_F(EvalTestFixture, doubled_rooks_with_blockers_file) {
    Board board = Board("8/r7/8/P7/8/8/r7/8");
    ASSERT_FALSE(_doubled_rooks(board, BLACK));
}


// Rook on open file test
TEST_F(EvalTestFixture, rook_on_open_file_start_pos) {
    Board board = Board();
    ASSERT_EQ(0, _rook_on_open_file(board, BLACK));
}

TEST_F(EvalTestFixture, empty_field_roo_on_open_file) {
    Board board = Board("8/8/1R6/3R4/8/8/8/7R");
    ASSERT_EQ(3, _rook_on_open_file(board, WHITE));
}

TEST_F(EvalTestFixture, blockers_rook_on_open_file) {
    Board board = Board("8/5R2/8/8/5r2/8/8/8");
    ASSERT_EQ(0, _rook_on_open_file(board, BLACK));
}


/** pawns test */
// isolated pawns
TEST_F(EvalTestFixture, islands_of_pawns) {
    Board board = Board("8/8/4P3/8/1P6/3P4/P7/8");
    ASSERT_EQ(0, _isolated_pawns(board, WHITE));
}

TEST_F(EvalTestFixture, max_isolated_pawns) {
    Board board = Board("8/7P/8/4P3/8/2P5/P7/8");
    ASSERT_EQ(4, _isolated_pawns(board, WHITE));
}

// connected pawns
TEST_F(EvalTestFixture, several_whtie_connected_pawns) {
    Board board = Board("8/8/7P/P7/3P4/4P3/3P4/8");
    ASSERT_EQ(2, _connected_pawns(board, WHITE));
}

TEST_F(EvalTestFixture, several_black_connected_pawns) {
    Board board = Board("8/7p/6p1/8/1pp5/pppp4/8/8");
    ASSERT_EQ(3, _connected_pawns(board, BLACK));
}

TEST_F(EvalTestFixture, many_doubled_pawns) {
    Board board = Board("8/8/8/8/P1P5/P1P5/P6P/P7");
    ASSERT_EQ(4, _doubled_pawns(board, WHITE));
}

TEST_F(EvalTestFixture, white_passed_pawns) {
    Board board = Board("8/PP6/6p1/P2Pp2p/krn5/1P6/5P1P/8");
    ASSERT_EQ(5, _passed_pawns(board, WHITE));
}

TEST_F(EvalTestFixture, black_passed_pawns) {
    Board board = Board("8/pp6/6p1/p2Pp2p/krn5/1p6/5p1P/8");
    ASSERT_EQ(6, _passed_pawns(board, BLACK));
}

TEST_F(EvalTestFixture, white_and_black_king_pawns_shield_true) {
    Board board = Board();
    bool actual = _king_pawns_shield(board, WHITE) &&
            _king_pawns_shield(board, BLACK);
    ASSERT_TRUE(actual);
}

TEST_F(EvalTestFixture, black_king_pawns_shield_false) {
    Board board = Board("k7/pp6/8/8/8/8/8/8");
    ASSERT_TRUE(_king_pawns_shield(board, BLACK));
}