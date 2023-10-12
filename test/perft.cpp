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
    int64_t perft(Board &board, int depth) {
        return 0;
    }

public:
    static void SetUpTestCase() {
        Rays::init();
        Attacks::init();
    }
};

// TODO Recode everything to perft method
//TEST_F(PerftFixture, max_possible_moves_in_position_1) {
//    // Arrange
//    Board board = Board("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 w - - 0");
//
//    // Act
//    Movegen::gen_moves(&board, &m_move_picker);
//    uint8_t actual = m_move_picker.get_moves();
//
//    // Assert
//    ASSERT_EQ(218, actual);
//}
//
//TEST_F(PerftFixture, max_possible_moves_in_position_2) {
//    // Arrange
//    Board board = Board("3Q4/1Q4Q1/4Q3/2Q4R/Q4Q2/3Q4/1Q4Rp/1K1BBNNk w - - 0");
//
//    // Act
//    Movegen::gen_moves(&board, &m_move_picker);
//    uint8_t actual = m_move_picker.get_moves();
//
//    // Assert
//    ASSERT_EQ(218, actual);
//}
//
//TEST_F(PerftFixture, check_en_passant_for_black) {
//    Board board = Board("rnbqkbnr/pppp1ppp/8/8/4pP2/8/PPPPP1PP/RNBQKBNR b KQkq f3 0");
//
//    Movegen::gen_moves(&board, &m_move_picker);
//    uint8_t actual = m_move_picker.get_moves();
//
//    ASSERT_EQ(30, actual);
//}
//
//TEST_F(PerftFixture, start_position) {
//    Board board = Board();
//
//    Movegen::gen_moves(&board, &m_move_picker);
//    uint8_t actual = m_move_picker.get_moves();
//
//    ASSERT_EQ(20, actual);
//}