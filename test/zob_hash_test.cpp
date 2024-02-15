#include <gtest/gtest.h>
#include "board.hpp"
#include "rays.hpp"

class ZobHashTestFixture : public ::testing::Test {
public:
    static void SetUpTestCase() {
        Rays::init();
        Attacks::init();
        ZobristHash::init();
    }
};

TEST_F(ZobHashTestFixture, make_move_block) {
    Board board = Board("r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3");
    Board expected = Board("r6r/1b2k2q/5b2/8/7B/8/8/R3K2R w KQ - 4");

    board.make(Move(g7, f6, BISHOP));
    ASSERT_EQ(expected, board);
}

TEST_F(ZobHashTestFixture, make_one_move) {
    Board board = Board("b7/8/8/8/8/8/8/1B6 w - - 0");
    Board expected = Board("8/8/8/8/B7/8/8/7b b - - 3");

    board.make(Move(b1, c2, BISHOP));
    board.make(Move(a8, h1, BISHOP));
    board.make(Move(c2, a4, BISHOP));

    ASSERT_EQ(expected, board);
}