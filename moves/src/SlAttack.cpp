#include "SlAttack.hpp"

bitboard SlAttack::RookMask[64];
bitboard SlAttack::RookAttack[64][4096];

bitboard SlAttack::BishopMask[64];
bitboard SlAttack::BishopAttack[64][1024];

uint64_t SlAttack::get_blockers(uint16_t index, uint64_t mask) {
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

bitboard SlAttack::calculate_rook_attacks(uint8_t cell, uint64_t blockers) {
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

void SlAttack::init_rook_mask() {
    for (uint8_t i = 0; i < 64; ++i) {
        RookMask[i] = (Rays::get_ray(NORTH, i) & ~ROW8)  |
                      (Rays::get_ray(EAST,  i) & ~COL_H) |
                      (Rays::get_ray(SOUTH, i) & ~ROW1)  |
                      (Rays::get_ray(WEST,  i) & ~COL_A);
    }
}

void SlAttack::init_rook_table() {
    for (uint8_t i = 0; i < 64; ++i)
        for (uint16_t j = 0; j < (ONE << RookBits[i]); ++j) {
            uint64_t blockers = get_blockers(j, RookMask[i]);
            RookAttack[i][blockers * RookMagics[i] >> (64 - RookBits[i])] = calculate_rook_attacks(i, blockers);
        }
}


bitboard SlAttack::calculate_bishop_attacks(uint8_t cell, uint64_t blockers) {
    bitboard temp, attack = ZERO;

    // NORTH EAST
    temp = Rays::get_ray(NORTH_EAST, cell);
    attack |= temp;
    if (temp & blockers)
        attack &= ~Rays::get_ray(NORTH_EAST, lsb(temp & blockers));

    // SOUTH EAST
    temp = Rays::get_ray(SOUTH_EAST, cell);
    attack |= temp;
    if (temp & blockers)
        attack &= ~Rays::get_ray(SOUTH_EAST, msb(temp & blockers));

    // SOUTH WEST
    temp = Rays::get_ray(SOUTH_WEST, cell);
    attack |= temp;
    if (temp & blockers)
        attack &= ~Rays::get_ray(SOUTH_WEST, msb(temp & blockers));

    // NORTH WEST
    temp = Rays::get_ray(NORTH_WEST, cell);
    attack |= temp;
    if (temp & blockers)
        attack &= ~Rays::get_ray(NORTH_WEST, lsb(temp & blockers));
    return attack;
}

void SlAttack::init_bishop_mask() {
    uint64_t invert_edges_mask = ~(ROW1 | ROW8 | COL_A | COL_H);
    for (uint8_t i = 0; i < 64; ++i) {
        BishopMask[i] = (Rays::get_ray(NORTH_EAST, i) | Rays::get_ray(SOUTH_EAST, i) |
                Rays::get_ray(SOUTH_WEST, i) | Rays::get_ray(NORTH_WEST, i)) & invert_edges_mask;
    }
}

void SlAttack::init_bishop_table() {
    for (uint8_t i = 0; i < 64; ++i)
        for (uint16_t j = 0; j < (ONE << BishopBits[i]); ++j) {
            uint64_t blockers = get_blockers(j, BishopMask[i]);
            BishopAttack[i][blockers * BishopMagics[i] >> (64 - BishopBits[i])] = calculate_bishop_attacks(i, blockers);
        }
}



void SlAttack::init() {
    // Order is important!! Mask, then Table
    init_rook_mask();
    init_rook_table();
    init_bishop_mask();
    init_bishop_table();
}

bitboard SlAttack::get_rook_attack(uint8_t cell, uint64_t blockers) {
    blockers &= RookMask[cell];
    uint64_t key = blockers * RookMagics[cell] >> (64 - RookBits[cell]);
    return RookAttack[cell][key];
}

bitboard SlAttack::get_bishop_attack(uint8_t cell, uint64_t blockers) {
    blockers &= BishopMask[cell];
    uint64_t key = blockers * BishopMagics[cell] >> (64 - BishopBits[cell]);
    return BishopAttack[cell][key];
}

bitboard SlAttack::get_queen_attack(uint8_t cell, uint64_t blockers) {
    return get_rook_attack(cell, blockers) | get_bishop_attack(cell, blockers);
}
