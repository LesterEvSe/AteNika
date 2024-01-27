#include "attacks.hpp"
#include "rays.hpp"

bitboard Attacks::hidden::_rook_mask[64];
bitboard Attacks::hidden::_rook_attacks[64][4096];

bitboard Attacks::hidden::_bishop_mask[64];
bitboard Attacks::hidden::_bishop_attacks[64][512];

bitboard Attacks::hidden::_pawn_attacks[COLOR_SIZE][64];
bitboard Attacks::hidden::_knight_attacks[64];
bitboard Attacks::hidden::_king_attacks[64];

void Attacks::init() {
    hidden::_init_rook_mask();
    hidden::_init_bishop_mask();
    hidden::_init_rook_attacks(); // using _rook_mask
    hidden::_init_bishop_attacks(); // using _bishop_mask

    hidden::_init_pawn_attacks();
    hidden::_init_knight_attacks();
    hidden::_init_king_attacks();
}

// convert from mask to blockers pieces
bitboard Attacks::hidden::_get_blockers(uint16_t index, bitboard mask) {
    uint64_t blockers = ZERO;
    uint8_t all = count_bits(mask);

    for (uint8_t i = 0; i < all; ++i) {
        uint8_t pos = pop_lsb(mask);

        if (index & (ONE << i))
            blockers |= (ONE << pos);
    }
    return blockers;
}

bitboard Attacks::hidden::_calculate_rook_attacks(uint8_t cell, bitboard blockers) {
    bitboard temp, attack = ZERO;

    // NORTH
    temp = Rays::get_ray(Rays::NORTH, cell);
    attack |= temp;
    if (temp & blockers)
        attack &= ~Rays::get_ray(Rays::NORTH, lsb(temp & blockers));

    // EAST
    temp = Rays::get_ray(Rays::EAST, cell);
    attack |= temp;
    if (temp & blockers)
        attack &= ~Rays::get_ray(Rays::EAST, lsb(temp & blockers));

    // SOUTH
    temp = Rays::get_ray(Rays::SOUTH, cell);
    attack |= temp;
    if (temp & blockers)
        attack &= ~Rays::get_ray(Rays::SOUTH, msb(temp & blockers));

    // WEST
    temp = Rays::get_ray(Rays::WEST, cell);
    attack |= temp;
    if (temp & blockers)
        attack &= ~Rays::get_ray(Rays::WEST, msb(temp & blockers));
    return attack;
}

bitboard Attacks::hidden::_calculate_bishop_attacks(uint8_t cell, bitboard blockers) {
    bitboard temp, attack = ZERO;

    // NORTH EAST
    temp = Rays::get_ray(Rays::NORTH_EAST, cell);
    attack |= temp;
    if (temp & blockers)
        attack &= ~Rays::get_ray(Rays::NORTH_EAST, lsb(temp & blockers));

    // SOUTH EAST
    temp = Rays::get_ray(Rays::SOUTH_EAST, cell);
    attack |= temp;
    if (temp & blockers)
        attack &= ~Rays::get_ray(Rays::SOUTH_EAST, msb(temp & blockers));

    // SOUTH WEST
    temp = Rays::get_ray(Rays::SOUTH_WEST, cell);
    attack |= temp;
    if (temp & blockers)
        attack &= ~Rays::get_ray(Rays::SOUTH_WEST, msb(temp & blockers));

    // NORTH WEST
    temp = Rays::get_ray(Rays::NORTH_WEST, cell);
    attack |= temp;
    if (temp & blockers)
        attack &= ~Rays::get_ray(Rays::NORTH_WEST, lsb(temp & blockers));
    return attack;
}

void Attacks::hidden::_init_rook_mask() {
    for (uint8_t i = 0; i < 64; ++i) {
        _rook_mask[i] = (Rays::get_ray(Rays::Direction::NORTH, i) & ~RANK_8) |
                        (Rays::get_ray(Rays::Direction::EAST,  i) & ~FILE_H) |
                        (Rays::get_ray(Rays::Direction::SOUTH, i) & ~RANK_1) |
                        (Rays::get_ray(Rays::Direction::WEST,  i) & ~FILE_A);
    }
}

void Attacks::hidden::_init_bishop_mask() {
    bitboard invert_edges_mask = ~(RANK_1 | RANK_8 | FILE_A | FILE_H);
    for (uint8_t i = 0; i < 64; ++i) {
        _bishop_mask[i] = (Rays::get_ray(Rays::Direction::NORTH_EAST, i) |
                           Rays::get_ray(Rays::Direction::SOUTH_EAST, i) |
                           Rays::get_ray(Rays::Direction::SOUTH_WEST, i) |
                           Rays::get_ray(Rays::Direction::NORTH_WEST, i)) & invert_edges_mask;
    }
}

void Attacks::hidden::_init_rook_attacks() {
    for (uint8_t i = 0; i < 64; ++i)
        for (uint16_t j = 0; j < (ONE << _rook_bits[i]); ++j) {
            bitboard blockers = _get_blockers(j, _rook_mask[i]);

            _rook_attacks[i][(blockers * _rook_magics[i]) >> (64 - _rook_bits[i])] =
                    _calculate_rook_attacks(i, blockers);
        }
}

void Attacks::hidden::_init_bishop_attacks() {
    for (uint8_t i = 0; i < 64; ++i)
        for (uint16_t j = 0; j < (ONE << _bishop_bits[i]); ++j) {
            bitboard blockers = _get_blockers(j, _bishop_mask[i]);

            _bishop_attacks[i][(blockers * _bishop_magics[i]) >> (64 - _bishop_bits[i])] =
                    _calculate_bishop_attacks(i, blockers);
        }
}

void Attacks::hidden::_init_pawn_attacks() {
    // We need all 8 rows, for under_attack method in Board class
    for (uint8_t i = 0; i < 64; ++i) {
        bitboard piece = ONE << i;

        _pawn_attacks[WHITE][i] = (piece << 7) & ~FILE_H | (piece << 9) & ~FILE_A;
        _pawn_attacks[BLACK][i] = (piece >> 7) & ~FILE_A | (piece >> 9) & ~FILE_H;
    }
}

void Attacks::hidden::_init_knight_attacks() {
    for (uint8_t i = 0; i < 64; ++i) {
        bitboard piece = ONE << i;

        // lr - left and right shift
        bitboard lr1 = (piece >> 1) & ~FILE_H | // left
                       (piece << 1) & ~FILE_A; // right
        bitboard lr2 = (piece >> 2) & ~(FILE_G | FILE_H) |
                       (piece << 2) & ~(FILE_A | FILE_B);
        _knight_attacks[i] = lr1 << 16 | lr1 >> 16 | lr2 << 8 | lr2 >> 8;
    }
}

void Attacks::hidden::_init_king_attacks() {
    for (uint8_t i = 0; i < 64; ++i) {
        bitboard piece = ONE << i;

        _king_attacks[i] = ((piece << 7) | (piece >> 1) | (piece >> 9)) & ~FILE_H |
                            (piece << 8) | (piece >> 8) |
                           ((piece << 9) | (piece << 1) | (piece >> 7)) & ~FILE_A;
    }
}

bitboard Attacks::get_rook_attacks(uint8_t cell, bitboard blockers) {
    blockers &= hidden::_rook_mask[cell];
    uint64_t key = (blockers * hidden::_rook_magics[cell]) >> (64 - hidden::_rook_bits[cell]);
    return hidden::_rook_attacks[cell][key];
}

bitboard Attacks::get_bishop_attacks(uint8_t cell, bitboard blockers) {
    blockers &= hidden::_bishop_mask[cell];
    uint64_t key = (blockers * hidden::_bishop_magics[cell]) >> (64 - hidden::_bishop_bits[cell]);
    return hidden::_bishop_attacks[cell][key];
}

bitboard Attacks::get_queen_attacks(uint8_t cell, bitboard blockers) {
    return get_rook_attacks(cell, blockers) | get_bishop_attacks(cell, blockers);
}

bitboard Attacks::get_pawn_attacks(Color color, uint8_t cell) { return hidden::_pawn_attacks[color][cell]; }
bitboard Attacks::get_knight_attacks(uint8_t cell) { return hidden::_knight_attacks[cell]; }
bitboard Attacks::get_king_attacks(uint8_t cell)   { return hidden::_king_attacks[cell];   }