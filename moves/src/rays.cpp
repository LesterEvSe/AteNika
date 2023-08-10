#include "rays.hpp"

uint64_t Rays::s_rays[8][64];

// The next 2 functions are auxiliary
uint64_t Rays::east_shift(uint64_t diag, uint8_t n) {
    for (uint8_t i = 0; i < n; ++i)
        diag = (diag << 1) & ~COL_A;
    return diag;
}

uint64_t Rays::west_shift(uint64_t diag, uint8_t n) {
    for (uint8_t i = 0; i < n; ++i)
        diag = diag >> 1 & ~COL_H;
    return diag;
}


void Rays::calculate_rays() {
    // Calculate Diagonals
    uint64_t bitA1 = ONE;
    uint64_t bitH1 = ONE << 7;
    uint64_t DIAG_A1_H8 = ZERO;
    uint64_t DIAG_A8_H1 = ZERO;

    for (uint8_t i = 0; i < 8; ++i) {
        DIAG_A1_H8 |= bitA1 << (9 * i);
        DIAG_A8_H1 |= bitH1 << (7 * i);
    }

    uint64_t one, east, west;
    for (uint8_t i = 0; i < 64; ++i) {
        if (i % 8 == 0) {
            one = ONE << i;
            east = ROW1 << i & ~one;
            west = ZERO;
        }

        s_rays[NORTH][i] = (COL_A & ~ROW1) << i;
        s_rays[SOUTH][i] = (COL_H & ~ROW8) >> (63 - i);
        s_rays[EAST][i] = east;
        s_rays[WEST][i] = west;

        east = east << 1 & ~COL_A;
        west |= one;
        one <<= 1;

        // i % 8 - get col
        // i / 8 - get row
        s_rays[NORTH_EAST][i] = east_shift(DIAG_A1_H8 & ~ROW1, i % 8) << (i - (i % 8));
        s_rays[NORTH_WEST][i] = west_shift(DIAG_A8_H1 & ~ROW1, 7 - (i % 8)) << (i - (i % 8));
        s_rays[SOUTH_EAST][i] = east_shift(DIAG_A8_H1 & ~ROW8, i % 8) >> ((7 - i / 8) * 8);
        s_rays[SOUTH_WEST][i] = west_shift(DIAG_A1_H8 & ~ROW8, 7 - (i % 8)) >> ((7 - i / 8) * 8);
    }
}

void Rays::init() {
    calculate_rays();
}
uint64_t Rays::get_ray(Direction side, uint8_t cell) {
    return s_rays[side][cell];
}
