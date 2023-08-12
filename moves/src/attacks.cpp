#include "attacks.hpp"
#include "sliding_attacks.hpp"

bitboard Attacks::KingAttacks[64];
bitboard Attacks::KnightAttacks[64];

void Attacks::calculate_king_attacks() {
    for (uint8_t i = 0; i < 64; ++i) {
        bitboard bit = ONE << i;
        KingAttacks[i] = (bit << 7 | bit >> 1 | bit >> 9) & ~COL_H |
                         (bit << 8 | bit >> 8) |
                         (bit << 9 | bit << 1 | bit >> 7) & ~COL_A;
    }
}

void Attacks::calculate_knight_attacks() {
    for (int i = 0; i < 64; ++i) {
        bitboard bit = ONE << i;

        // lr - left and right shift
        bitboard lr1 = (bit >> 1) & ~COL_H | // left
                       (bit << 1) & ~COL_A;  // right
        bitboard lr2 = (bit >> 2) & ~(COL_G | COL_H) |
                       (bit << 2) & ~(COL_A | COL_B);
        KnightAttacks[i] = lr1 << 16 | lr1 >> 16 | lr2 << 8 | lr2 >> 8;
    }
}



void Attacks::init() {
    calculate_king_attacks();
    calculate_knight_attacks();
}

bitboard Attacks::get_king_moves(const Pieces &pieces, uint8_t cell, bool side) {
    return KingAttacks[cell] & pieces.m_reverse_side[side];
}

bitboard Attacks::get_king_captures(const Pieces& pieces, uint8_t cell, bool side) {
    return KingAttacks[cell] & pieces.m_side[get_reverse_side(side)];
}

bitboard Attacks::get_knight_moves(const Pieces &pieces, uint8_t cell, bool side) {
    return KnightAttacks[cell] & pieces.m_reverse_side[side];
}

bitboard Attacks::get_knight_captures(const Pieces& pieces, uint8_t cell, bool side) {
    return KnightAttacks[cell] & pieces.m_side[get_reverse_side(side)];
}


bitboard Attacks::get_white_pawn_default_moves(const Pieces& pieces) {
    return pieces.m_pieces_bitboards[WHITE][PAWN] << 8 & pieces.m_empty;
}
bitboard Attacks::get_black_pawn_default_moves(const Pieces& pieces) {
    return pieces.m_pieces_bitboards[BLACK][PAWN] >> 8 & pieces.m_empty;
}

bitboard Attacks::get_white_pawn_long_moves(const Pieces& pieces) {
    bitboard default_white_moves = get_white_pawn_default_moves(pieces);
    return (default_white_moves & ROW2) << 8 & pieces.m_empty;
}
bitboard Attacks::get_black_pawn_long_moves(const Pieces& pieces) {
    bitboard default_black_moves = get_black_pawn_default_moves(pieces);
    return (default_black_moves & ROW5) >> 8 & pieces.m_empty;
}

bitboard Attacks::get_white_pawn_left_captures(const Pieces& pieces, bool all_captures) {
    bitboard mask = pieces.m_pieces_bitboards[WHITE][PAWN] << 7 & ~COL_H;
    if (all_captures)
        mask &= pieces.m_side[BLACK];
    return mask;
}
bitboard Attacks::get_black_pawn_left_captures(const Pieces& pieces, bool all_captures) {
    bitboard mask = pieces.m_pieces_bitboards[BLACK][PAWN] >> 9 & ~COL_H;
    if (all_captures)
        mask &= pieces.m_side[WHITE];
    return mask;
}

bitboard Attacks::get_white_pawn_right_captures(const Pieces& pieces, bool all_captures) {
    bitboard mask = pieces.m_pieces_bitboards[WHITE][PAWN] << 9 & ~COL_A;
    if (all_captures)
        mask &= pieces.m_side[BLACK];
    return mask;
}
bitboard Attacks::get_black_pawn_right_captures(const Pieces& pieces, bool all_captures) {
    bitboard mask = pieces.m_pieces_bitboards[BLACK][PAWN] << 7 & ~COL_A;
    if (all_captures)
        mask &= pieces.m_side[WHITE];
    return mask;
}

// Unknown situation with rook, bishop and queen. It's sliding piecess
//bool Attacks::under_attack(const Pieces &pieces, uint8_t cell, bool side) {
//    bitboard pawn_attacks;
//    if (side == WHITE) {
//        pawn_attacks = get_black_pawn_left_captures(pieces, true) |
//                get_black_pawn_right_captures(pieces, true);
//    } else {
//        pawn_attacks = get_white_pawn_left_captures(pieces, true) |
//                 get_white_pawn_right_captures(pieces, true);
//    }
//
//    if (get_bit(pawn_attacks, cell)) return true;
//    if (get_knight_captures(pieces, cell, side) & pieces.m_pieces_bitboards[get_reverse_side(side)][KNIGHT]) return true;
//    if (get_king_captures(pieces, cell, side) & pieces.m_pieces_bitboards[get_reverse_side(side)][KING]) return true;
//    if (SlAttack::get_bishop)
//    return false;
//}
