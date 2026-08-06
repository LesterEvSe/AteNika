#include "eval.hpp"

bitboard Eval::detail::_wp_passed_mask[64];
bitboard Eval::detail::_bp_passed_mask[64];

void Eval::init() {
    for (uint8_t i = 0; i < 8; ++i) {
        detail::_wp_passed_mask[i] =
            (detail::COL[i] >> 1) & ~FILE_H | detail::COL[i] | (detail::COL[i] << 1) & ~FILE_A;
        detail::_bp_passed_mask[56 + i] = detail::_wp_passed_mask[i];

        detail::_wp_passed_mask[i] &= ~RANK_1;
        detail::_bp_passed_mask[56 + i] &= ~RANK_8;
    }

    for (uint8_t i = 8; i < 64; ++i) {
        detail::_wp_passed_mask[i] = detail::_wp_passed_mask[i - 8] << 8;
        detail::_bp_passed_mask[63 - i] = detail::_bp_passed_mask[63 - i + 8] >> 8;
    }
}

// Test Pos. Eval from vice
// 8/6R1/2k5/6P1/8/8/4nP2/6K1 w - - 1 41
bool Eval::detail::material_draw(const Board &board) {
    if (board.get_pieces(WHITE, QUEEN) || board.get_pieces(BLACK, QUEEN))
        return false;

    const uint8_t wR = count_bits(board.get_pieces(WHITE, ROOK));
    const uint8_t bR = count_bits(board.get_pieces(BLACK, ROOK));

    const uint8_t wB = count_bits(board.get_pieces(WHITE, BISHOP));
    const uint8_t bB = count_bits(board.get_pieces(BLACK, BISHOP));

    const uint8_t wN = count_bits(board.get_pieces(WHITE, KNIGHT));
    const uint8_t bN = count_bits(board.get_pieces(BLACK, KNIGHT));

    if (wR || bR) {
        if (wR == 1 && bR == 1)
            return wN + wB < 2 && bN + bB < 2;
        if (wR == 1 && bR == 0)
            return wN + wB == 0 && (bN + bB == 1 || bN + bB == 2);
        if (wR == 0 && bR == 1)
            return bN + bB == 0 && (wN + wB == 1 || wN + wB == 2);
        return false;
    }

    if (!wB && !bB)
        return wN < 3 && bN < 3;
    if (!wN && !bN)
        return std::abs(wB - bB) < 2;
    if (wN < 3 && !wB || wB == 1 && !wN)
        return bN < 3 && !bB || bB == 1 && !bN;
    return false;
}

int32_t Eval::evaluate(const Board &board) {
    int32_t score = 0;
    int32_t mat_white = 0;
    int32_t mat_black = 0;

    const bitboard bP = board.get_pieces(BLACK, PAWN); // for white passed (opposite color)
    const bitboard wP = board.get_pieces(WHITE, PAWN); // for black passed (here too)
    const bitboard all_pawns = bP | wP; // for open and semi open files for rooks and queens

    /*
    if (!bP && !wP && detail::material_draw(board))
        return 0;
    */

    // Pawns
    bitboard pieces = board.get_pieces(WHITE, PAWN);
    while (pieces) {
        const uint8_t sq = pop_lsb(pieces);
        score += detail::PST[PAWN][detail::FLIP[sq]];
        mat_white += detail::MATERIAL_BONUS[PAWN];

        score += (pieces & detail::ISOLATED_PAWNS_MASK[get_file(sq)]) ? 0 : detail::ISOLATED_PAWN;
        score += (bP & detail::_wp_passed_mask[sq] ? 0 : detail::PASSED_PAWNS[get_rank(sq)]);
    }

    pieces = board.get_pieces(BLACK, PAWN);
    while (pieces) {
        const uint8_t sq = pop_lsb(pieces);
        score -= detail::PST[PAWN][sq];
        mat_black += detail::MATERIAL_BONUS[PAWN];

        score -= (pieces & detail::ISOLATED_PAWNS_MASK[get_file(sq)]) ? 0 : detail::ISOLATED_PAWN;
        score -= (wP & detail::_bp_passed_mask[sq] ? 0 : detail::PASSED_PAWNS[7 - get_rank(sq)]);
    }

    // Knights
    pieces = board.get_pieces(WHITE, KNIGHT);
    while (pieces) {
        score += detail::PST[KNIGHT][detail::FLIP[pop_lsb(pieces)]];
        mat_white += detail::MATERIAL_BONUS[KNIGHT];
    }

    pieces = board.get_pieces(BLACK, KNIGHT);
    while (pieces) {
        score -= detail::PST[KNIGHT][pop_lsb(pieces)];
        mat_black += detail::MATERIAL_BONUS[KNIGHT];
    }

    // Bishops
    pieces = board.get_pieces(WHITE, BISHOP);
    score += ((pieces & WHITE_SQUARES) && (pieces & BLACK_SQUARES)) ? detail::BISHOP_PAIR : 0;

    while (pieces) {
        score += detail::PST[BISHOP][detail::FLIP[pop_lsb(pieces)]];
        mat_white += detail::MATERIAL_BONUS[BISHOP];
    }

    pieces = board.get_pieces(BLACK, BISHOP);
    score -= ((pieces & WHITE_SQUARES) && (pieces & BLACK_SQUARES)) ? detail::BISHOP_PAIR : 0;

    while (pieces) {
        score -= detail::PST[BISHOP][pop_lsb(pieces)];
        mat_black += detail::MATERIAL_BONUS[BISHOP];
    }

    // Rooks
    pieces = board.get_pieces(WHITE, ROOK);
    while (pieces) {
        const uint8_t sq = detail::FLIP[pop_lsb(pieces)];
        score += detail::PST[ROOK][sq];
        mat_white += detail::MATERIAL_BONUS[ROOK];

        const bitboard file = detail::COL[get_file(sq)];
        if (!(all_pawns & file))
            score += detail::ROOK_OPEN_FILE;
        else if (!(wP & file))
            score += detail::ROOK_SEMI_OPEN_FILE;
    }

    pieces = board.get_pieces(BLACK, ROOK);
    while (pieces) {
        const uint8_t sq = pop_lsb(pieces);
        score -= detail::PST[ROOK][sq];
        mat_black += detail::MATERIAL_BONUS[ROOK];

        const bitboard file = detail::COL[get_file(sq)];
        if (!(all_pawns & file))
            score -= detail::ROOK_OPEN_FILE;
        else if (!(bP & file))
            score -= detail::ROOK_SEMI_OPEN_FILE;
    }

    // Queens
    pieces = board.get_pieces(WHITE, QUEEN);
    while (pieces) {
        const uint8_t sq = detail::FLIP[pop_lsb(pieces)];
        score += detail::PST[QUEEN][sq];
        mat_white += detail::MATERIAL_BONUS[QUEEN];

        const bitboard file = detail::COL[get_file(sq)];
        if (!(all_pawns & file))
            score += detail::QUEEN_OPEN_FILE;
        else if (!(wP & file))
            score += detail::QUEEN_SEMI_OPEN_FILE;
    }

    pieces = board.get_pieces(BLACK, QUEEN);
    while (pieces) {
        const uint8_t sq = pop_lsb(pieces);
        score -= detail::PST[QUEEN][sq];
        mat_black += detail::MATERIAL_BONUS[QUEEN];

        const bitboard file = detail::COL[get_file(sq)];
        if (!(all_pawns & file))
            score -= detail::QUEEN_OPEN_FILE;
        else if (!(bP & file))
            score -= detail::QUEEN_SEMI_OPEN_FILE;
    }

    // Kings
    score += mat_white > detail::ENDGAME_MAT
                 ? detail::KING_M[detail::FLIP[lsb(board.get_pieces(WHITE, KING))]]
                 : detail::KING_E[detail::FLIP[lsb(board.get_pieces(WHITE, KING))]];

    score -= mat_black > detail::ENDGAME_MAT ? detail::KING_M[lsb(board.get_pieces(BLACK, KING))]
                                             : detail::KING_E[lsb(board.get_pieces(BLACK, KING))];

    score += mat_white - mat_black;
    return board.get_curr_move() == WHITE ? score : -score;
}