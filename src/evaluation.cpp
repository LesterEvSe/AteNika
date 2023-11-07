#include "evaluation.hpp"

bitboard Evaluation::hidden::_pawns_shield_mask[COLOR_SIZE][64];

void Evaluation::init() {
    for (uint8_t i = 0; i < 64; ++i) {
        bitboard king = ONE << i;

        hidden::_pawns_shield_mask[WHITE][i] = (king << 7) & ~FILE_H |
                                               (king << 8) |
                                               (king << 9) & ~FILE_A;

        hidden::_pawns_shield_mask[BLACK][i] = (king >> 7) & ~FILE_A |
                                               (king >> 8) |
                                               (king >> 9) & ~FILE_H;
    }
}

bool Evaluation::hidden::_has_bishop_pair(const Board &board, Color color) {
    bitboard bishops = board.get_pieces(color, BISHOP);
    return ((bishops & WHITE_SQUARES) && (bishops & BLACK_SQUARES));
}

// Test
uint8_t Evaluation::hidden::_rook_on_open_file(const Board &board, Color color) {
    bitboard rooks = board.get_pieces(color, ROOK);
    bitboard all = board.get_all_pieces();
    uint8_t quantity = 0;

    while (rooks) {
        uint8_t square = pop_lsb(rooks);
        bitboard curr_rook = ONE << square;
        if ((ROW[get_rank(square)] & all) == curr_rook ||
            (COL[get_file(square)] & all) == curr_rook)
            ++quantity;
    }
    return quantity;
}

// TODO need to test
bool Evaluation::hidden::_doubled_rooks(const Board &board, Color color) {
    bitboard rooks = board.get_pieces(color, ROOK);
    if (count_bits(rooks) < 2)
        return false;

    uint8_t rook_square = pop_lsb(rooks);
    bitboard row = ROW[get_rank(rook_square)];
    bitboard rook_in_row = rooks & row;
    if (rook_in_row && (board.get_all_pieces() & row ^ rook_in_row) == rook_in_row)
        return true;

    bitboard col = COL[get_file(rook_square)];
    bitboard rook_in_col = rooks & col;
    if (rook_in_col && (board.get_all_pieces() & col ^ rook_in_col) == rook_in_col)
        return true;
    return false;

}

uint8_t Evaluation::hidden::_isolated_pawns(const Board &board, Color color);
uint8_t Evaluation::hidden::_connected_pawns(const Board &board, Color color) {
    bitboard pawns = board.get_pieces(color, PAWN);
    bitboard copy_pawns = pawns;
    uint8_t connected = 0;

    while (copy_pawns) {
        uint8_t square = pop_lsb(copy_pawns);
        if (Attacks::get_pawn_attacks(color, square) & pawns)
            ++connected;
    }
    return connected;
}

uint8_t Evaluation::hidden::_doubled_pawns(const Board &board, Color color) {
    bitboard pawns = board.get_pieces(color, PAWN);
    uint8_t doubled = 0;

    for (int i = 0; i < 8; ++i)
        doubled += count_bits(pawns & COL[i]) - 1;
    return doubled;
}

// think about Rays and recode NULL_FILES_AND_NEIGHBORS array
uint8_t Evaluation::hidden::_passed_pawns(const Board &board, Color color) {

}

bool Evaluation::hidden::_king_pawns_shield(const Board &board, Color color) {
    bitboard pawns = board.get_pieces(color, PAWN);
    bitboard shield = _pawns_shield_mask[OPENING][board.get_pieces(color, KING)];
    return (shield & pawns) == shield;
}

int32_t Evaluation::get_material_eval(PieceType piece) {
    return hidden::MATERIAL_BONUS[OPENING][piece];
}