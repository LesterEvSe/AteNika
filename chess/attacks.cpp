#include "attacks.hpp"
#include "rays.hpp"
#include "bitfunc.hpp"

bitboard Attacks::_rook_mask[64];
bitboard Attacks::_rook_attacks[64][4096];

bitboard Attacks::_bishop_mask[64];
bitboard Attacks::_bishop_attacks[64][512];

bitboard Attacks::_pawn_attacks[2][64];
bitboard Attacks::_knight_attacks[64];
bitboard Attacks::_king_attacks[64];

void Attacks::init() {
    _init_rook_mask();
    _init_bishop_mask();
    _init_rook_attacks(); // using _rook_mask
    _init_bishop_attacks(); // using _bishop_mask

    _init_pawn_attacks();
    _init_knight_attacks();
    _init_king_attacks();
}

void Attacks::_init_rook_mask() {
    for (uint8_t i = 0; i < 64; ++i) {
        _rook_mask[i] = Rays::get_ray(Rays::Direction::NORTH, i) |
                        Rays::get_ray(Rays::Direction::WEST,  i) |
                        Rays::get_ray(Rays::Direction::SOUTH, i) |
                        Rays::get_ray(Rays::Direction::EAST,  i);
    }
}

void Attacks::_init_bishop_mask() {
    for (uint8_t i = 0; i < 64; ++i) {
        _bishop_mask[i] = Rays::get_ray(Rays::Direction::NORTH_EAST, i) |
                          Rays::get_ray(Rays::Direction::SOUTH_EAST, i) |
                          Rays::get_ray(Rays::Direction::SOUTH_WEST, i) |
                          Rays::get_ray(Rays::Direction::NORTH_WEST, i);
    }
}

void Attacks::_init_rook_attacks() {

}

void Attacks::_init_bishop_attacks() {

}

void Attacks::_init_pawn_attacks() {
    // In the first and eighth rows, pawns turn into other pieces,
    // so for them, we do not need to calculate attacks
    for (uint8_t i = 8; i < 56; ++i) {
        bitboard piece = ONE << i;

        _pawn_attacks[WHITE][i] = (piece << 7) & ~FILE_H | (piece << 9) & ~FILE_A;
        _pawn_attacks[BLACK][i] = (piece >> 7) & ~FILE_A | (piece >> 9) & ~FILE_H;
    }
}

void Attacks::_init_knight_attacks() {
    for (uint8_t i = 0; i < 64; ++i) {
        bitboard piece = ONE << i;

        // lr - left and right shift
        bitboard lr1 = (piece >> 1) & ~FILE_H | // left
                       (piece << 1) & ~FILE_A; // right
        bitboard lr2 = (piece >> 2) & ~(FILE_G | FILE_H) |
                       (piece << 2) & (FILE_A | FILE_B);
        _knight_attacks[i] = lr1 << 16 | lr1 >> 16 | lr2 << 8 | lr2 >> 8;
    }
}

void Attacks::_init_king_attacks() {
    for (uint8_t i = 0; i < 64; ++i) {
        bitboard piece = ONE << i;

        _king_attacks[i] = ((piece << 7) | (piece >> 1) | (piece >> 9)) & ~FILE_H |
                            (piece << 8) | (piece << 9) |
                           ((piece << 9) | (piece << 1) | (piece >> 7)) & ~FILE_A;
    }
}

bitboard Attacks::_get_rook_attacks(PieceType type, uint8_t cell, bitboard blockers) {
    return 0;
}

bitboard Attacks::_get_bishop_attacks(PieceType type, uint8_t cell, bitboard blockers) {
    return 0;
}


bitboard Attacks::get_sliding_attacks(PieceType type, uint8_t cell, bitboard blockers) {
    switch (type) {
        case BISHOP : return _get_bishop_attacks(type, cell, blockers);
        case ROOK   : return _get_rook_attacks(type, cell, blockers);
        case QUEEN  : return _get_bishop_attacks(type, cell, blockers) | _get_rook_attacks(type, cell, blockers);
        default     : error("Unknown type of sliding piece");
    }
    // Won't get it here
    return 0;
}

bitboard Attacks::get_non_sliding_attacks(Color color, PieceType type, uint8_t cell) {
    switch (type) {
        case PAWN   : return _pawn_attacks[color][cell];
        case KNIGHT : return _knight_attacks[cell];
        case KING   : return _king_attacks[cell];
        default     : error("Unknown type of non sliding piece");
    }
    // Won't get it here
    return 0;
}