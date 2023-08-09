#include "SlAttacks.hpp"

bitboard SlAttacks::RookMask[64];
bitboard SlAttacks::RookAttacks[64][4096];

uint64_t SlAttacks::get_blockers(uint16_t index, uint64_t mask) {
    uint64_t blockers = ZERO;
    uint8_t bits1 = count1(mask);

    for (uint8_t i = 0; i < bits1; ++i) {
        uint8_t bit_pos = lsb(mask);
        set0(mask, bit_pos);

        if (index & (ONE << i))
            blockers |= ONE << bit_pos;
    }
    return blockers;
}

bitboard SlAttacks::calculate_rook_attacks(uint8_t cell, uint64_t blockers) {
    bitboard temp, attack = ZERO;

    // NORTH
    temp = Rays::get_ray(NORTH, cell);
    attack |= temp;
    if (temp & blockers)
        attack &= ~Rays::get_ray(NORTH, lsb(temp & blockers));

    // EAST
    temp = Rays::get_ray(EAST, cell);
    attack |= temp;
    if (temp & blockers)
        attack &= ~Rays::get_ray(EAST, lsb(temp & blockers));

    // SOUTH
    temp = Rays::get_ray(SOUTH, cell);
    attack |= temp;
    if (temp & blockers)
        attack &= ~Rays::get_ray(SOUTH, msb(temp & blockers));

    // WEST
    temp = Rays::get_ray(WEST, cell);
    attack |= temp;
    if (temp & blockers)
        attack &= ~Rays::get_ray(WEST, msb(temp & blockers));
    return attack;
}

void SlAttacks::init_rook_mask() {
    for (uint8_t i = 0; i < 64; ++i) {
        RookMask[i] = (Rays::get_ray(NORTH, i) & ~ROW8)  |
                      (Rays::get_ray(EAST,  i) & ~COL_H) |
                      (Rays::get_ray(SOUTH, i) & ~ROW1)  |
                      (Rays::get_ray(WEST,  i) & ~COL_A);
    }
}

void SlAttacks::init_rook_table() {
    for (uint8_t i = 0; i < 64; ++i)
        for (uint64_t j = 0; j < (uint64_t(1) << RookBits[i]); ++j) {
            uint64_t blockers = get_blockers(j, RookMask[i]);
            RookAttacks[i][blockers * RookMagics[i] >> (64 - RookBits[i])] =
                    calculate_rook_attacks(i, blockers);
        }
}


void SlAttacks::init() {
    init_rook_mask();
    init_rook_table();
}

bitboard SlAttacks::get_rook_attacks(uint8_t ind, uint64_t blockers) {
    return RookAttacks[ind][blockers * RookMagics[ind] >> (64 - RookBits[ind])];
}
