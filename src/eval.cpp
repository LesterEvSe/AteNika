#include "eval.hpp"

int32_t Eval::evaluate(const Board &board) {
    int32_t score = 0;
    int32_t mat_white = 0;
    int32_t mat_black = 0;

    for (PieceType type : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN})
    {
        bitboard white = board.get_pieces(WHITE, type);
        while (white) {
            score += hidden::PST[type][hidden::FLIP[pop_lsb(white)]];
            mat_white += hidden::MATERIAL_BONUS[type];
        }

        bitboard black = board.get_pieces(BLACK, type);
        while (black) {
            score -= hidden::PST[type][pop_lsb(black)];
            mat_black += hidden::MATERIAL_BONUS[type];
        }
    }


    score += mat_white > hidden::ENDGAME_MAT ?
            hidden::KING_M[hidden::FLIP[lsb(board.get_pieces(WHITE, KING))]] :
            hidden::KING_E[hidden::FLIP[lsb(board.get_pieces(WHITE, KING))]];

    score -= mat_black > hidden::ENDGAME_MAT ?
             hidden::KING_M[lsb(board.get_pieces(BLACK, KING))] :
             hidden::KING_E[lsb(board.get_pieces(BLACK, KING))];

    return board.get_curr_move() == WHITE ? score : -score;
}