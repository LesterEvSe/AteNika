#include "attacks.hpp"
#include "rays.hpp"
#include "bitfunc.hpp"

bitboard Attacks::_rook_mask[64];
bitboard Attacks::_rook_attacks[64][4096];

bitboard Attacks::_bishop_mask[64];
bitboard Attacks::_bishop_attacks[64][512];

bitboard Attacks::_knight_attacks[64];
bitboard Attacks::_king_attacks[64];

void Attacks::init() {
    _init_rook_mask();
    _init_bishop_mask();
    _init_rook_attacks(); // using _rook_mask
    _init_bishop_attacks(); // using _bishop_mask

    _init_knight_attacks();
    _init_king_attacks();
}

// convert from mask to blockers pieces
bitboard Attacks::_get_blockers(uint16_t index, bitboard mask) {
    uint64_t blockers = ZERO;
    uint8_t all = count_bits(mask);

    for (uint8_t i = 0; i < all; ++i) {
        uint8_t pos = lsb(mask);
        mask &= ~(ONE << pos); // set0

        if (index & (ONE << i))
            blockers |= (ONE << pos);
    }
    return blockers;
}

bitboard Attacks::_calculate_rook_attacks(uint8_t cell, bitboard blockers) {
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

bitboard Attacks::_calculate_bishop_attacks(uint8_t cell, bitboard blockers) {
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

void Attacks::_init_rook_mask() {
    for (uint8_t i = 0; i < 64; ++i) {
        _rook_mask[i] = (Rays::get_ray(Rays::Direction::NORTH, i) & ~RANK_8) |
                        (Rays::get_ray(Rays::Direction::EAST,  i) & ~FILE_H) |
                        (Rays::get_ray(Rays::Direction::SOUTH, i) & ~RANK_1) |
                        (Rays::get_ray(Rays::Direction::WEST,  i) & ~FILE_A);
    }
}

void Attacks::_init_bishop_mask() {
    bitboard invert_edges_mask = ~(RANK_1 | RANK_8 | FILE_A | FILE_H);
    for (uint8_t i = 0; i < 64; ++i) {
        _bishop_mask[i] = (Rays::get_ray(Rays::Direction::NORTH_EAST, i) |
                           Rays::get_ray(Rays::Direction::SOUTH_EAST, i) |
                           Rays::get_ray(Rays::Direction::SOUTH_WEST, i) |
                           Rays::get_ray(Rays::Direction::NORTH_WEST, i)) & invert_edges_mask;
    }
}

void Attacks::_init_rook_attacks() {
    for (uint8_t i = 0; i < 64; ++i)
        for (uint16_t j = 0; j < (ONE << _rook_bits[i]); ++j) {
            bitboard blockers = _get_blockers(j, _rook_mask[i]);

            _rook_attacks[i][(blockers * _rook_magics[i]) >> (64 - _rook_bits[i])] =
                    _calculate_rook_attacks(i, blockers);
        }
}

void Attacks::_init_bishop_attacks() {
    for (uint8_t i = 0; i < 64; ++i)
        for (uint16_t j = 0; j < (ONE << _bishop_bits[i]); ++j) {
            bitboard blockers = _get_blockers(j, _bishop_mask[i]);

            _bishop_attacks[i][(blockers * _bishop_magics[i]) >> (64 - _bishop_bits[i])] =
                    _calculate_bishop_attacks(i, blockers);
        }
}

void Attacks::_init_knight_attacks() {
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

void Attacks::_init_king_attacks() {
    for (uint8_t i = 0; i < 64; ++i) {
        bitboard piece = ONE << i;

        _king_attacks[i] = ((piece << 7) | (piece >> 1) | (piece >> 9)) & ~FILE_H |
                            (piece << 8) | (piece >> 8) |
                           ((piece << 9) | (piece << 1) | (piece >> 7)) & ~FILE_A;
    }
}

bitboard Attacks::get_rook_attacks(uint8_t cell, bitboard blockers) {
    blockers &= _rook_mask[cell];
    uint64_t key = (blockers * _rook_magics[cell]) >> (64 - _rook_bits[cell]);
    return _rook_attacks[cell][key];
}

bitboard Attacks::get_bishop_attacks(uint8_t cell, bitboard blockers) {
    blockers &= _bishop_mask[cell];
    uint64_t key = (blockers * _bishop_magics[cell]) >> (64 - _bishop_bits[cell]);
    return _bishop_attacks[cell][key];
}

bitboard Attacks::get_queen_attacks(uint8_t cell, bitboard blockers) {
    return get_rook_attacks(cell, blockers) | get_bishop_attacks(cell, blockers);
}

bitboard Attacks::get_knight_attacks(uint8_t cell)            { return _knight_attacks[cell];      }
bitboard Attacks::get_king_attacks(uint8_t cell)              { return _king_attacks[cell];        }
