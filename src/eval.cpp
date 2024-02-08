#include "eval.hpp"

template<GamePhase phase, Color color, bool with_pawns>
int32_t Eval::hidden::_eval_material(const Board &board) {
    int32_t score = 0;
    if (with_pawns)
        score += MATERIAL_BONUS[phase][PAWN] * count_bits(board.get_pieces(color, PAWN));
    score += MATERIAL_BONUS[phase][KNIGHT] * count_bits(board.get_pieces(color, KNIGHT));
    score += MATERIAL_BONUS[phase][BISHOP] * count_bits(board.get_pieces(color, BISHOP));
    score += MATERIAL_BONUS[phase][ROOK]   * count_bits(board.get_pieces(color, ROOK));
    score += MATERIAL_BONUS[phase][QUEEN]  * count_bits(board.get_pieces(color, QUEEN));
    return score;
}

// opening_limit = 15258, endgame_limit = 3915
int32_t Eval::hidden::_tapered_eval(const Board &board) {
    int32_t non_pawn_material = _eval_material<OPENING, WHITE, false>(board) - _eval_material<OPENING, BLACK, false>(board);
    non_pawn_material = std::max(std::min(3915, non_pawn_material), 15258);
    return ((non_pawn_material - 15258) << 7) / 11343; // 15258 - 3915
}

template<GamePhase phase>
int32_t Eval::hidden::_eval_for_phase(const Board &board) {
    int32_t score = 0;
    score += _eval_material<phase, WHITE, true>(board) - _eval_material<phase, BLACK, true>(board);
    score += board.get_pst_score(WHITE, phase) - board.get_pst_score(BLACK, phase);
    return score;
}

int32_t Eval::evaluate(const Board &board) {
    int32_t opening = hidden::_eval_for_phase<OPENING>(board);
    int32_t endgame = hidden::_eval_for_phase<ENDGAME>(board);
    int32_t phase = hidden::_tapered_eval(board);
    // endgame = endgame * scale_factor(board) / 64;
    int32_t score = ((opening * phase + (endgame * (128 - phase))) >> 7);

    // tempo
    score += board.get_curr_move() == WHITE ? 28 : -28;
    return score * (100 - board.get_ply()) / 100;
//    return board.get_curr_move() == WHITE ? score : -score;
}