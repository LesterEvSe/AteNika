#ifndef ATENIKA_MOVEGEN_HPP
#define ATENIKA_MOVEGEN_HPP

#include "board.hpp"
#include "move_list.hpp"

class Movegen
{
private:
    const Board &m_board;
    MoveList m_moves;
    MoveList m_legal_moves;

    void add_moves(uint8_t from, bitboard moves, PieceType piece);
    void add_attacks(uint8_t from, bitboard attacks, PieceType move_piece, Color defender);

    void gen_moves();
    void gen_legal_moves();

    void gen_white_moves();
    void gen_black_moves();

    void gen_white_pawn_moves_and_captures();
    void gen_black_pawn_moves_and_captures();

    void gen_pawn_promotion(uint8_t from, uint8_t to, Move::Flag flag = Move::QUIET, PieceType captured_piece = NONE);
    void gen_white_pawn_moves();
    void gen_black_pawn_moves();

    void gen_white_left_pawn_captures();
    void gen_black_left_pawn_captures();
    void gen_white_right_pawn_captures();
    void gen_black_right_pawn_captures();

    void gen_white_king_moves();
    void gen_black_king_moves();

    void gen_white_knight_moves();
    void gen_black_knight_moves();

    void gen_white_rook_moves();
    void gen_black_rook_moves();

    void gen_white_bishop_moves();
    void gen_black_bishop_moves();

    void gen_white_queen_moves();
    void gen_black_queen_moves();

public:
    explicit Movegen(const Board &board);
    MoveList &get_legal_moves();
};

#endif //ATENIKA_MOVEGEN_HPP
