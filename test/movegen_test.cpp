#include <gtest/gtest.h>
#include "rays.hpp"
#include "board.hpp"
#include "movepicker.hpp"
#include "movegen.hpp"

// using Stockfish 16 to calculation the expected value
class MovegenTestFixture : public testing::Test
{
protected:
    MovePicker m_move_picker;

public:
    MovegenTestFixture() = default;
    static void SetUpTestCase() {
        Rays::init();
        Attacks::init();
        MovePicker::init();
    }
};

TEST_F(MovegenTestFixture, start_position) {
    Board board = Board();

    Movegen::gen_moves(&board, &m_move_picker);
    uint8_t actual = m_move_picker.get_moves();

    ASSERT_EQ(20, actual);
}

TEST_F(MovegenTestFixture, max_possible_moves_in_position_1) {
    // Arrange
    Board board = Board("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 w - - 0");

    // Act
    Movegen::gen_moves(&board, &m_move_picker);
    uint8_t actual = m_move_picker.get_moves();

    // Assert
    ASSERT_EQ(218, actual);
}

TEST_F(MovegenTestFixture, max_possible_moves_in_position_2) {
    // Arrange
    Board board = Board("3Q4/1Q4Q1/4Q3/2Q4R/Q4Q2/3Q4/1Q4Rp/1K1BBNNk w - - 0");

    // Act
    Movegen::gen_moves(&board, &m_move_picker);
    uint8_t actual = m_move_picker.get_moves();

    // Assert
    ASSERT_EQ(218, actual);
}

TEST_F(MovegenTestFixture, engines_problem_position_for_white) {
    Board board = Board("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 0");

    Movegen::gen_moves(&board, &m_move_picker);
    uint8_t actual = m_move_picker.get_moves();

    ASSERT_EQ(-1, actual);
}

TEST_F(MovegenTestFixture, end_of_mittelspiel_all_pieces_for_white) {
    Board board = Board("1r4k1/5qp1/p3pn1p/4Rp2/2pPPP2/2P4P/r2N2P1/2Q3RK w - - 0");

    Movegen::gen_moves(&board, &m_move_picker);
    uint8_t actual = m_move_picker.get_moves();

    ASSERT_EQ(-1, actual);
}

TEST_F(MovegenTestFixture, peak_mittelspiel_all_pieces_for_white) {
    Board board = Board("r1bq1rk1/2p1bppp/p1np1n2/1p2p3/4P3/1BP2N1P/PP1P1PP1/RNBQR1K1 w - - 0");

    Movegen::gen_moves(&board, &m_move_picker);
    uint8_t actual = m_move_picker.get_moves();

    ASSERT_EQ(-1, actual);
}

TEST_F(MovegenTestFixture, end_of_mittelspiel_all_pieces_for_black) {
    Board board = Board("1r4k1/5qp1/p3pn1p/4Rp2/2pPPP2/2P4P/r2N2P1/2Q3RK b - - 0");

    Movegen::gen_moves(&board, &m_move_picker);
    uint8_t actual = m_move_picker.get_moves();

    ASSERT_EQ(-1, actual);
}

TEST_F(MovegenTestFixture, peak_mittelspiel_all_pieces_for_black) {
    Board board = Board("r1bq1rk1/2p1bppp/p1np1n2/1p2p3/4P3/1BP2N1P/PP1P1PP1/RNBQR1K1 b - - 0");

    Movegen::gen_moves(&board, &m_move_picker);
    uint8_t actual = m_move_picker.get_moves();

    ASSERT_EQ(-1, actual);
}

TEST_F(MovegenTestFixture, check_en_passant_for_white) {
    Board board = Board("r1bq1rk1/2p1bppp/p1np1n2/1pP1p3/4P3/1B3N1P/PP1P1PP1/RNBQR1K1 w - b6 0");

    Movegen::gen_moves(&board, &m_move_picker);
    uint8_t actual = m_move_picker.get_moves();

    ASSERT_EQ(-1, actual);
}

TEST_F(MovegenTestFixture, check_en_passant_for_black) {
    Board board = Board("rnbqkbnr/pppp1ppp/8/8/4pP2/8/PPPPP1PP/RNBQKBNR b KQkq f3 0");
    std::cout << board;

    Movegen::gen_moves(&board, &m_move_picker);
    uint8_t actual = m_move_picker.get_moves();

    ASSERT_EQ(30, actual);
}