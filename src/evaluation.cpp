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

int32_t Evaluation::get_material_eval(PieceType piece) {
    return hidden::MATERIAL_BONUS[OPENING][piece];
}