#include <gtest/gtest.h>
#include "board.hpp"
#include "rays.hpp"

class BoardTestFixture : public testing::Test
{
protected:
    static Board board;

public:
    static void SetUpTestCase() {
        Rays::init();
        Attacks::init();
        std::cout << board;
    }
};

Board BoardTestFixture::board = Board("rnbq1k1r/pp1Pbppp/2p5/6K1/2B5/8/PPP1NnPP/RNBQ3R w KQ - 0");

// Check get_pieces method for pawns
TEST_F(BoardTestFixture, white_pawns_position) {
    // Arrange & Act
    bitboard actual = board.get_pieces(WHITE, PAWN);

    // Assert
    ASSERT_EQ(0x0800000000C700, actual);
}

TEST_F(BoardTestFixture, black_pawns_position) {
    bitboard actual = board.get_pieces(BLACK, PAWN);
    ASSERT_EQ(0xE3040000000000, actual);
}

// Testing under_attack method
TEST_F(BoardTestFixture, white_pawn_under_attack) {
    // under black knight attack
    bool actual = board.under_attack(WHITE, get_cell("d7"));
    ASSERT_TRUE(actual);
}

TEST_F(BoardTestFixture, black_pawn_under_attack) {
    bool actual = board.under_attack(BLACK, get_cell("f7"));
    ASSERT_TRUE(actual);
}

TEST_F(BoardTestFixture, empty_cell_under_attack_of_black_piece) {
    bool actual = board.under_attack(WHITE, get_cell("f4"));
    ASSERT_FALSE(actual);
}

TEST_F(BoardTestFixture, empty_cell_under_attack_of_black_king) {
    bool actual = board.under_attack(BLACK, get_cell("h5"));
    ASSERT_TRUE(actual);
}

TEST_F(BoardTestFixture, cell_under_attack_of_white_pawn) {
    bool actual = board.under_attack(WHITE, get_cell("a3"));
    ASSERT_TRUE(actual);
}

TEST_F(BoardTestFixture, cell_under_attack_of_white_rook) {
    bool actual = board.under_attack(BLACK, 15); // h2 cell
    ASSERT_TRUE(actual);
}

TEST_F(BoardTestFixture, cell_under_attack_of_black_rook) {
    bool actual = board.under_attack(WHITE, get_cell("h7"));
    ASSERT_TRUE(actual);
}

TEST_F(BoardTestFixture, empty_cell_under_attack_of_black) {
    bool actual = board.under_attack(BLACK, get_cell("b4"));
    ASSERT_FALSE(actual);
}

TEST_F(BoardTestFixture, empty_cell_under_attack_of_black_queen) {
    bool actual = board.under_attack(WHITE, get_cell("a5"));
    ASSERT_TRUE(actual);
}

TEST_F(BoardTestFixture, black_king_in_check) {
    bool actual = board.king_in_check(BLACK);
    ASSERT_FALSE(actual);
}

TEST_F(BoardTestFixture, white_king_in_check) {
    bool actual = board.king_in_check(WHITE);
    ASSERT_TRUE(actual);
}

// Testing castling methods
TEST_F(BoardTestFixture, white_king_side_castle) {
    bool actual = board.get_white_ks_castle();
    ASSERT_TRUE(actual);
}

TEST_F(BoardTestFixture, white_queen_side_castle) {
    bool actual = board.get_white_qs_castle();
    ASSERT_TRUE(actual);
}

TEST_F(BoardTestFixture, black_king_side_castle) {
    bool actual = board.get_black_ks_castle();
    ASSERT_FALSE(actual);
}

TEST_F(BoardTestFixture, black_queen_side_castle) {
    bool actual = board.get_black_qs_castle();
    ASSERT_FALSE(actual);
}

// get_piece_at(Color, uint8_t) method
TEST_F(BoardTestFixture, TestExit) {
    // Arrange
    std::string str = "white has not piece at e4 cell";

    // testing::ExitedWithCode data type
    auto error = testing::ExitedWithCode(1);

    auto lambda = []() -> PieceType {
        return board.get_piece_at(WHITE, get_cell("e4"));
    };

    // Act & Assert
    ASSERT_EXIT(lambda(), error, str);
}

TEST_F(BoardTestFixture, get_white_king) {
    PieceType actual = board.get_piece_at(WHITE, get_cell("g5"));
    ASSERT_EQ(KING, actual);
}

TEST_F(BoardTestFixture, get_black_bishop) {
    PieceType actual = board.get_piece_at(BLACK, get_cell("e7"));
    ASSERT_EQ(BISHOP, actual);
}