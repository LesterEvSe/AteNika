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
    // Arrange & Act
    bitboard actual = Attacks::get_queen_attacks(e4, 0);

    // Assert
    ASSERT_EQ(0x11925438EF385492, actual);
}

TEST_F(AttacksTestFixture, queen_with_blockers) {
    // Arrange
    bitboard blockers = 0x0004120843280816;

    // Act
    bitboard actual = Attacks::get_queen_attacks(e4, blockers);

    // Assert
    ASSERT_EQ(0x8050386E381010, actual);
}

// White Pawn
TEST_F(AttacksTestFixture, white_pawn_on_the_left_edge) {
    // Arrange & Act
    bitboard actual = Attacks::get_pawn_attacks(WHITE, a2);

    // Assert
    ASSERT_EQ(0x020000, actual);
}

TEST_F(AttacksTestFixture, white_pawn_on_the_right_edge) {
    bitboard actual = Attacks::get_pawn_attacks(WHITE, h3);
    ASSERT_EQ(0x40000000, actual);
}

TEST_F(AttacksTestFixture, white_pawn_in_the_center) {
    bitboard actual = Attacks::get_pawn_attacks(WHITE, c2);
    ASSERT_EQ(0x0A0000, actual);
}

TEST_F(AttacksTestFixture, white_pawn_in_the_first_rank) {
    bitboard actual = Attacks::get_pawn_attacks(WHITE, e1);
    ASSERT_EQ(0x2800, actual);
}

// Black Pawn
TEST_F(AttacksTestFixture, black_pawn_on_the_left_edge) {
    bitboard actual = Attacks::get_pawn_attacks(BLACK, a2);
    ASSERT_EQ(0x02, actual);
}

TEST_F(AttacksTestFixture, black_pawn_on_the_right_edge) {
    bitboard actual = Attacks::get_pawn_attacks(BLACK, h3);
    ASSERT_EQ(0x4000, actual);
}

TEST_F(AttacksTestFixture, black_pawn_in_the_center) {
    bitboard actual = Attacks::get_pawn_attacks(BLACK, c2);
    ASSERT_EQ(0x0A, actual);
}

TEST_F(AttacksTestFixture, black_pawn_in_the_first_rank) {
    bitboard actual = Attacks::get_pawn_attacks(BLACK, d8);
    ASSERT_EQ(0x14000000000000, actual);
}

// King
TEST_F(AttacksTestFixture, king_in_the_corner) {
    bitboard actual = Attacks::get_king_attacks(h1);
    ASSERT_EQ(0xC040, actual);
}

TEST_F(AttacksTestFixture, king_in_the_left_border) {
    bitboard actual = Attacks::get_king_attacks(a5);
    ASSERT_EQ(0x030203000000, actual);
}

TEST_F(AttacksTestFixture, king_in_the_right_border) {
    bitboard actual = Attacks::get_king_attacks(h3);
    ASSERT_EQ(0xC040C000, actual);
}

TEST_F(AttacksTestFixture, king_in_the_center) {
    bitboard actual = Attacks::get_king_attacks(d3);
    ASSERT_EQ(0x1C141C00, actual);
}

// Knight
TEST_F(AttacksTestFixture, knight_in_the_corner) {
    bitboard actual = Attacks::get_knight_attacks(a1);
    ASSERT_EQ(0x020400, actual);
}

TEST_F(AttacksTestFixture, knight_in_the_left_border) {
    bitboard actual = Attacks::get_knight_attacks(a4);
    ASSERT_EQ(0x020400040200, actual);
}

TEST_F(AttacksTestFixture, knigth_in_the_FILE_G) {
    bitboard actual = Attacks::get_knight_attacks(g4);
    ASSERT_EQ(0xA0100010A000, actual);
}

TEST_F(AttacksTestFixture, knight_in_the_center) {
    bitboard actual = Attacks::get_knight_attacks(d4);
    ASSERT_EQ(0x142200221400, actual);
}
