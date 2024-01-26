#include "eval.hpp"

bitboard Eval::hidden::_pawns_shield_mask[COLOR_SIZE][64];
bitboard Eval::hidden::_passed_pawns_mask[COLOR_SIZE][64];

void Eval::init() {
    // pawns_shield_mask init
    for (uint8_t i = 0; i < 64; ++i) {
        bitboard king = ONE << i;

        hidden::_pawns_shield_mask[WHITE][i] = (king << 7) & ~FILE_H |
                                               (king << 8) |
                                               (king << 9) & ~FILE_A;

        hidden::_pawns_shield_mask[BLACK][i] = (king >> 7) & ~FILE_A |
                                               (king >> 8) |
                                               (king >> 9) & ~FILE_H;
    }

    // passed_pawns_mask init
    for (uint8_t i = 0; i < 8; ++i) {
        hidden::_passed_pawns_mask[WHITE][i] = (hidden::COL[i] >> 1) & ~FILE_H |
                                                hidden::COL[i] |
                                               (hidden::COL[i] << 1) & ~FILE_A;
        hidden::_passed_pawns_mask[BLACK][56 + i] = hidden::_passed_pawns_mask[WHITE][i];

        hidden::_passed_pawns_mask[WHITE][i] &= ~RANK_1;
        hidden::_passed_pawns_mask[BLACK][56 + i] &= ~RANK_8;
    }

    for (uint8_t i = 8; i < 64; ++i) {
        hidden::_passed_pawns_mask[WHITE][i] = hidden::_passed_pawns_mask[WHITE][i-8] << 8;
        hidden::_passed_pawns_mask[BLACK][63-i] = hidden::_passed_pawns_mask[BLACK][63-i+8] >> 8;
    }
}

bool Eval::hidden::_has_bishop_pair(const Board &board, Color color) {
    bitboard bishops = board.get_pieces(color, BISHOP);
    return ((bishops & WHITE_SQUARES) && (bishops & BLACK_SQUARES));
}

bool Eval::hidden::_doubled_rooks(const Board &board, Color color) {
    bitboard all = board.get_all_pieces();
    bitboard rooks = board.get_pieces(color, ROOK);
    bitboard temp = rooks;

    while (temp) {
        uint8_t square = pop_lsb(temp);
        if (rooks & Attacks::get_rook_attacks(square, all))
            return true;
    }
    return false;
}

uint8_t Eval::hidden::_rook_on_open_file(const Board &board, Color color) {
    bitboard rooks = board.get_pieces(color, ROOK);
    bitboard all = board.get_all_pieces();
    uint8_t quantity = 0;

    while (rooks) {
        uint8_t square = pop_lsb(rooks);
        bitboard curr_rook = ONE << square;
        if ((COL[get_file(square)] & all) == curr_rook)
            ++quantity;
    }
    return quantity;
}



uint8_t Eval::hidden::_isolated_pawns(const Board &board, Color color) {
    bitboard pawns = board.get_pieces(color, PAWN);
    bitboard copy_pawns = pawns;
    uint8_t isolated = 0;

    while (copy_pawns) {
        uint8_t col = get_file(pop_lsb(copy_pawns));
        if (count_bits(pawns & FILES_AND_NEIGHBORS[col]) == 1)
            ++isolated;
    }
    return isolated;
}

uint8_t Eval::hidden::_connected_pawns(const Board &board, Color color) {
    bitboard pawns = board.get_pieces(color, PAWN);
    bitboard copy_pawns = pawns;
    uint8_t connected = 0;

    while (copy_pawns) {
        uint8_t square = pop_lsb(copy_pawns);

        // if one pawn protect two, then only one pawn connected
        if (Attacks::get_pawn_attacks(color, square) & pawns)
            ++connected;
    }
    return connected;
}

uint8_t Eval::hidden::_doubled_pawns(const Board &board, Color color) {
    bitboard pawns = board.get_pieces(color, PAWN);
    uint8_t doubled = 0;

    for (int i = 0; i < 8; ++i) {
        uint8_t pawns_on_file = count_bits(pawns & COL[i]);
        doubled += pawns_on_file ? pawns_on_file-1 : 0;
    }
    return doubled;
}

uint8_t Eval::hidden::_passed_pawns(const Board &board, Color color) {
    bitboard pawns = board.get_pieces(color, PAWN);
    bitboard enemy_pawns = board.get_pieces(get_opposite_move(color), PAWN);
    uint8_t passed = 0;

    while (pawns) {
        uint8_t sq = pop_lsb(pawns);
        if (!(_passed_pawns_mask[color][sq] & enemy_pawns))
            ++passed;
    }
    return passed;
}

bool Eval::hidden::_king_pawns_shield(const Board &board, Color color) {
    bitboard pawns = board.get_pieces(color, PAWN);
    bitboard shield = _pawns_shield_mask[OPENING][lsb(board.get_pieces(color, KING))];
    return (shield & pawns) == shield;
}

int32_t Eval::hidden::_get_pawn_eval(const Board &board, Color color, GamePhase phase) {
    Color opposite = get_opposite_move(color);

    int32_t eval = 0;
    eval += ISOLATED_PAWNS[phase] * _isolated_pawns(board, color);
    eval -= ISOLATED_PAWNS[phase] * _isolated_pawns(board, opposite);

    eval += CONNECTED_PAWNS[phase] * _connected_pawns(board, color);
    eval -= CONNECTED_PAWNS[phase] * _connected_pawns(board, opposite);

    eval += DOUBLED_PAWNS[phase] * _doubled_pawns(board, color);
    eval -= DOUBLED_PAWNS[phase] * _doubled_pawns(board, opposite);

    eval += PASSED_PAWNS[phase] * _passed_pawns(board, color);
    eval -= PASSED_PAWNS[phase] * _passed_pawns(board, opposite);

    if (phase == ENDGAME)
        return eval;

    eval += _king_pawns_shield(board, color) ? KING_PAWNS_SHIELD[phase] : 0;
    eval -= _king_pawns_shield(board, opposite) ? KING_PAWNS_SHIELD[phase] : 0;
    return eval;
}



int32_t Eval::hidden::_evaluate_material(const Board &board, Color color, GamePhase phase) {
    int32_t eval = 0;
    Color opposite = get_opposite_move(color);

    for (uint8_t i = 0; i < PIECE_SIZE-1; ++i) {
        eval += count_bits(board.get_pieces(color, PIECES[i])) * MATERIAL_BONUS[phase][PIECES[i]];
        eval -= count_bits(board.get_pieces(opposite, PIECES[i])) * MATERIAL_BONUS[phase][PIECES[i]];
    }
    return eval;
}
int32_t Eval::hidden::_evaluate_mobility(const Board &board, Color color, GamePhase phase) {
    int32_t eval = 0;
    Color opposite = get_opposite_move(color);

    for (PieceType type : PIECES) {
        eval += count_bits(board.get_pieces(color, type)) * MOBILITY_BONUS[phase][type];
        eval -= count_bits(board.get_pieces(opposite, type)) * MOBILITY_BONUS[phase][type];
    }
    return eval;
}

int32_t Eval::hidden::_phase_evaluation(const Board &board, Color color, GamePhase phase) {
    int32_t eval = 0;
    eval += _get_pawn_eval(board, color, phase);
    eval += _evaluate_material(board, color, phase);
    eval += _evaluate_mobility(board, color, phase);

    Color opposite = get_opposite_move(color);
    eval += _has_bishop_pair(board, color) ? BISHOP_PAIR[phase] : 0;
    eval -= _has_bishop_pair(board, opposite) ? BISHOP_PAIR[phase] : 0;

    eval += _doubled_rooks(board, color) ? DOUBLED_ROOKS[phase] : 0;
    eval -= _doubled_rooks(board, opposite) ? DOUBLED_ROOKS[phase] : 0;

    eval += board.get_pst().get_eval(color, phase);
    eval -= board.get_pst().get_eval(opposite, phase);

    if (phase == ENDGAME)
        return eval;

    eval += ROOK_OPEN_FILE[phase] * (_rook_on_open_file(board, color) - _rook_on_open_file(board, opposite));
    return eval;
}

int32_t Eval::hidden::_fast_phase_evaluation(const Board &board, Color color, GamePhase phase) {
    int32_t eval = 0;
    eval += _evaluate_material(board, color, phase);
    eval += _evaluate_mobility(board, color, phase);

    eval += board.get_pst().get_eval(color, phase);
    eval -= board.get_pst().get_eval(board.get_opponent_move(), phase);
    return eval;
}

int32_t Eval::hidden::_calculate_phase(const Board &board) {
    int32_t phase = TOTAL_PHASE;
    for (uint8_t i = 1; i < PIECE_SIZE-1; ++i) {
        phase -= count_bits(board.get_pieces(WHITE, PIECES[i])) * PHASE_WEIGHTS[i];
        phase -= count_bits(board.get_pieces(BLACK, PIECES[i])) * PHASE_WEIGHTS[i];
    }
    return (phase * MAX_PHASE + HALF_TOTAL_PHASE) / TOTAL_PHASE;
}

int32_t Eval::get_material(PieceType type) {
    return hidden::MATERIAL_BONUS[OPENING][type];
}

template int32_t Eval::evaluate<Eval::FAST>(const Board &board, Color color);
template int32_t Eval::evaluate<Eval::STATIC>(const Board &board, Color color);

// To speed up code and eliminate unnecessary runtime checks
template<Eval::Evaluation eval>
int32_t Eval::evaluate(const Board &board, Color color) {
    int32_t opening, endgame;
    if (eval == FAST) {
        opening = hidden::_fast_phase_evaluation(board, color, OPENING);
        endgame = hidden::_fast_phase_evaluation(board, color, ENDGAME);
    } else {
        opening = hidden::_phase_evaluation(board, color, OPENING);
        endgame = hidden::_phase_evaluation(board, color, ENDGAME);
    }

    int32_t phase = hidden::_calculate_phase(board);
    return (opening * (hidden::MAX_PHASE - phase) + endgame * phase) / hidden::MAX_PHASE;
}