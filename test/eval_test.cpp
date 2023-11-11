#include <gtest/gtest.h>
#include "rays.hpp"
#include "evaluation.hpp"

class EvalTestFixture : public testing::Test
{
public:
    static void SetUpTestCase() {
        Rays::init();
        Attacks::init();
        PieceTables::init();
        Evaluation::init();
    }
};

// It is hard enough testing only evaluation function,
// that's why I am testing hidden functions
TEST_F(EvalTestFixture, doubled_rooks_empty_field) {
    Board board = Board("8/8/8/8/8/R7/8/4R3");

    // Act
    bool doubled = Evaluation::hidden::_doubled_rooks(board, WHITE);

    // Assert
    ASSERT_FALSE(doubled);
}