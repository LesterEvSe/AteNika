#include "pawn.hpp"

bitboard Pawn::_pawn_attacks[2][64];

void Pawn::init_pawn_attacks() {
    // In the first and eighth rows, pawns turn into other pieces,
    // so for them, we do not need to calculate attacks
    for (uint8_t i = 8; i < 56; ++i) {
        bitboard piece = ONE << i;

        _pawn_attacks[WHITE][i] = (piece << 7) & ~FILE_H | (piece << 9) & ~FILE_A;
        _pawn_attacks[BLACK][i] = (piece >> 7) & ~FILE_A | (piece >> 9) & ~FILE_H;
    }
}

bitboard Pawn::get_moves(bitboard pawns, bitboard blockers, Color color) {
    // If after the move pawn stayed on the RANK_3 for WHITE
    // and RANK_6 row for BLACK
    blockers = ~blockers;
    switch (color)
    {
        case WHITE: {
            bitboard one_step = (pawns << 8) & blockers;
            return one_step | ((one_step & RANK_3) << 8) & blockers;
        }
        default: {
            bitboard one_step = (pawns >> 8) & blockers;
            return one_step | ((one_step & RANK_6) >> 8) & blockers;
        }
    }
}


bitboard Pawn::get_attacks(uint8_t cell, Color color) {
    return _pawn_attacks[color][cell];
}

bitboard Pawn::get_left_attacks(bitboard pawns, Color color) {
    switch (color) {
        case WHITE: return (pawns << 7) & ~FILE_H;
        default:    return (pawns >> 9) & ~FILE_H;
    }
}

bitboard Pawn::get_right_attacks(bitboard pawns, Color color) {
    switch (color) {
        case WHITE: return (pawns << 9) & ~FILE_A;
        default   : return (pawns >> 7) & ~FILE_A;
    }
}