#include <gtest/gtest.h>
#include "rays.hpp"
#include "board.hpp"
#include "search.hpp"

class MateFixture : public testing::Test
{
public:
    static void SetUpTestCase() {
        init_bits_pre_calculation();
        ZobristHash::init();
        Rays::init();
        Attacks::init();
        Search::init();
    }
};

TEST_F(MateFixture, mate_in_3_vice_lesson_60) {
    // Arrange
    Board board = Board("2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0");

    // Act
    Search::iter_deep(board, false);

    // Assert
    ASSERT_EQ("white mate in 3", Search::get_best_score());
}