#include <gtest/gtest.h>
#include "rays.hpp"
#include "attacks.hpp"

class AttacksTestFixture : public testing::Test
{
public:
    static void SetUpTestCase() {
        // Order is important
        Rays::init();
        Attacks::init();
    }
};

// IMPORTANT!!!
// We do not need bishop and rook attacks, because of queen use both

// Queen Attacks
TEST_F(AttacksTestFixture, queen_in_the_center_of_empty_board) {
    // Arrange
    uint8_t cell = 28; // 4E

    // Act
    bitboard actual = Attacks::get_queen_attacks(cell, 0);

    // Assert
    ASSERT_EQ(0x11925438EF385492, actual);
}

TEST_F(AttacksTestFixture, queen_with_blockers) {
    // Arrange
    bitboard blockers = 0x0004120843280816;
    uint8_t cell = 28; // 4E

    // Act
    bitboard actual = Attacks::get_queen_attacks(cell, blockers);

    // Assert
    ASSERT_EQ(0x8050386E381010, actual);
}

// White Pawn
TEST_F(AttacksTestFixture, white_pawn_on_the_left_edge) {
    // Arrange & Act
    bitboard actual = Attacks::get_pawn_attacks(WHITE, 8);

    // Assert
    ASSERT_EQ(0x020000, actual);
}

TEST_F(AttacksTestFixture, white_pawn_on_the_right_edge) {
    bitboard actual = Attacks::get_pawn_attacks(WHITE, 23);
    ASSERT_EQ(0x40000000, actual);
}

TEST_F(AttacksTestFixture, white_pawn_in_the_center) {
    bitboard actual = Attacks::get_pawn_attacks(WHITE, 10);
    ASSERT_EQ(0x0A0000, actual);
}

TEST_F(AttacksTestFixture, white_pawn_in_the_first_rank) {
    bitboard actual = Attacks::get_pawn_attacks(WHITE, 4);
    ASSERT_EQ(0x2800, actual);
}

// Black Pawn
TEST_F(AttacksTestFixture, black_pawn_on_the_left_edge) {
    bitboard actual = Attacks::get_pawn_attacks(BLACK, 8);
    ASSERT_EQ(0x02, actual);
}

TEST_F(AttacksTestFixture, black_pawn_on_the_right_edge) {
    bitboard actual = Attacks::get_pawn_attacks(BLACK, 23);
    ASSERT_EQ(0x4000, actual);
}

TEST_F(AttacksTestFixture, black_pawn_in_the_center) {
    bitboard actual = Attacks::get_pawn_attacks(BLACK, 10);
    ASSERT_EQ(0x0A, actual);
}

TEST_F(AttacksTestFixture, black_pawn_in_the_first_rank) {
    bitboard actual = Attacks::get_pawn_attacks(BLACK, 59);
    ASSERT_EQ(0x14000000000000, actual);
}

// King
TEST_F(AttacksTestFixture, king_in_the_corner) {
    bitboard actual = Attacks::get_king_attacks(7);
    ASSERT_EQ(0xC040, actual);
}

TEST_F(AttacksTestFixture, king_in_the_left_border) {
    bitboard actual = Attacks::get_king_attacks(32);
    ASSERT_EQ(0x030203000000, actual);
}

TEST_F(AttacksTestFixture, king_in_the_right_border) {
    bitboard actual = Attacks::get_king_attacks(23);
    ASSERT_EQ(0xC040C000, actual);
}

TEST_F(AttacksTestFixture, king_in_the_center) {
    bitboard actual = Attacks::get_king_attacks(19);
    ASSERT_EQ(0x1C141C00, actual);
}

// Knight
TEST_F(AttacksTestFixture, knight_in_the_corner) {
    bitboard actual = Attacks::get_knight_attacks(0);
    ASSERT_EQ(0x020400, actual);
}

TEST_F(AttacksTestFixture, knight_in_the_left_border) {
    bitboard actual = Attacks::get_knight_attacks(24);
    ASSERT_EQ(0x020400040200, actual);
}

TEST_F(AttacksTestFixture, knigth_in_the_FILE_G) {
    bitboard actual = Attacks::get_knight_attacks(30);
    ASSERT_EQ(0xA0100010A000, actual);
}

TEST_F(AttacksTestFixture, knight_in_the_center) {
    bitboard actual = Attacks::get_knight_attacks(27);
    ASSERT_EQ(0x142200221400, actual);
}
