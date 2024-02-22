#include <gtest/gtest.h>
#include "board.hpp"
#include "rays.hpp"
#include "bitfunc.hpp"

class BoardTestFixture : public ::testing::Test
{
protected:
    static Board board;

public:
    static void SetUpTestCase() {
        init_bits_pre_calculation();
        ZobristHash::init();
        Rays::init();
        Attacks::init();
        std::cout << board;
    }
};

Board BoardTestFixture::board = Board("rnbq1k1r/pp1Pbppp/2p5/6K1/2B5/8/PPP1NnPP/RNBQ3R w KQ - 36 17");

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
    bool actual = board.under_attack(WHITE, d7);
    ASSERT_TRUE(actual);
}

TEST_F(BoardTestFixture, black_pawn_under_attack) {
    bool actual = board.under_attack(BLACK, f7);
    ASSERT_TRUE(actual);
}

TEST_F(BoardTestFixture, empty_cell_under_attack_of_black_piece) {
    bool actual = board.under_attack(WHITE, f4);
    ASSERT_FALSE(actual);
}

TEST_F(BoardTestFixture, empty_cell_under_attack_of_black_king) {
    bool actual = board.under_attack(BLACK, h5);
    ASSERT_TRUE(actual);
}

TEST_F(BoardTestFixture, cell_under_attack_of_white_pawn) {
    bool actual = board.under_attack(WHITE, a3);
    ASSERT_TRUE(actual);
}

TEST_F(BoardTestFixture, cell_under_attack_of_white_rook) {
    bool actual = board.under_attack(BLACK, h2);
    ASSERT_TRUE(actual);
}

TEST_F(BoardTestFixture, cell_under_attack_of_black_rook) {
    bool actual = board.under_attack(WHITE, h7);
    ASSERT_TRUE(actual);
}

TEST_F(BoardTestFixture, empty_cell_under_attack_of_black) {
    bool actual = board.under_attack(BLACK, b4);
    ASSERT_FALSE(actual);
}

TEST_F(BoardTestFixture, empty_cell_under_attack_of_black_queen) {
    bool actual = board.under_attack(WHITE, a5);
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
        return board.get_piece_at(WHITE, e4);
    };

    // Act & Assert
    ASSERT_EXIT(lambda(), error, str);
}

TEST_F(BoardTestFixture, get_white_king) {
    PieceType actual = board.get_piece_at(WHITE, g5);
    ASSERT_EQ(KING, actual);
}

TEST_F(BoardTestFixture, get_black_bishop) {
    PieceType actual = board.get_piece_at(BLACK, e7);
    ASSERT_EQ(BISHOP, actual);
}

TEST_F(BoardTestFixture, en_passant) {
    Board board = Board("8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0 0");
    uint8_t en_passant_cell = board.get_en_passant();

    ASSERT_EQ(d3, en_passant_cell);
}

TEST_F(BoardTestFixture, get_fen) {
    std::string expected = "rnbq1k1r/pp1Pbppp/2p5/6K1/2B5/8/PPP1NnPP/RNBQ3R w KQ - 36 17";
    ASSERT_EQ(expected, board.get_fen());
}

TEST_F(BoardTestFixture, get_fen_without_castling_and_with_en_passant) {
    Board board = Board("rnbq1k1r/pp1Pbppp/2p5/6K1/2B5/8/PPP1NnPP/RNBQ3R w - e3 0 0");
    std::string expected = "rnbq1k1r/pp1Pbppp/2p5/6K1/2B5/8/PPP1NnPP/RNBQ3R w - e3 0 0";

    ASSERT_EQ(expected, board.get_fen());
}

TEST_F(BoardTestFixture, get_fen_full_castling) {
    Board board = Board("rnbq1k1r/pp1Pbppp/2p5/6K1/2B5/8/PPP1NnPP/RNBQ3R b KQkq - 0 0");
    std::string expected = "rnbq1k1r/pp1Pbppp/2p5/6K1/2B5/8/PPP1NnPP/RNBQ3R b KQkq - 0 0";

    ASSERT_EQ(expected, board.get_fen());
}