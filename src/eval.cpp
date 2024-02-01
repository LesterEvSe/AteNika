#include "eval.hpp"

int32_t Eval::hidden::eval_material(const Board &board, Color color) {
    int32_t score = 0;
    score += MATERIAL_BONUS[PAWN] * count_bits(board.get_pieces(color, PAWN));
    score += MATERIAL_BONUS[KNIGHT] * count_bits(board.get_pieces(color, KNIGHT));
    score += MATERIAL_BONUS[BISHOP] * count_bits(board.get_pieces(color, BISHOP));
    score += MATERIAL_BONUS[ROOK] * count_bits(board.get_pieces(color, ROOK));
    score += MATERIAL_BONUS[QUEEN] * count_bits(board.get_pieces(color, QUEEN));
    return score;
}

int32_t Eval::evaluate(const Board &board) {
    int32_t score = 0;
    score += hidden::eval_material(board, WHITE) - hidden::eval_material(board, BLACK);
    return board.get_curr_move() == WHITE ? score : -score;
}