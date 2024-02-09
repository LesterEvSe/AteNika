#include "eval.hpp"

template<Color color>
int32_t Eval::hidden::_eval_material(const Board &board) {
    int32_t score = 0;
    score += MATERIAL_BONUS[PAWN]   * count_bits(board.get_pieces(color, PAWN));
    score += MATERIAL_BONUS[KNIGHT] * count_bits(board.get_pieces(color, KNIGHT));
    score += MATERIAL_BONUS[BISHOP] * count_bits(board.get_pieces(color, BISHOP));
    score += MATERIAL_BONUS[ROOK]   * count_bits(board.get_pieces(color, ROOK));
    score += MATERIAL_BONUS[QUEEN]  * count_bits(board.get_pieces(color, QUEEN));
    return score;
}

// TODO change all evaluation to standard, because it's working too bad
int32_t Eval::evaluate(const Board &board) {
    int32_t score = 0;
    score += hidden::_eval_material<WHITE>(board) - hidden::_eval_material<BLACK>(board);
    score += board.get_pst_score(WHITE, OPENING) - board.get_pst_score(BLACK, OPENING);
    return board.get_curr_move() == WHITE ? score : -score;
}