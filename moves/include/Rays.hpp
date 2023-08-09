#ifndef CHESSAI_RAYS_HPP
#define CHESSAI_RAYS_HPP

#include "Bitboard.hpp"

enum Direction : uint8_t {
    NORTH = 0,
    SOUTH = 1,
    EAST  = 2,
    WEST  = 3,

    NORTH_EAST = 4,
    NORTH_WEST = 5,
    SOUTH_EAST = 6,
    SOUTH_WEST = 7,
};

/**
 *    N
 *  W   E
 *    S
 */

// The next 2 functions are auxiliary
static constexpr uint64_t east_shift(uint64_t diag, uint8_t n) {
    for (uint8_t i = 0; i < n; ++i)
        diag = (diag << 1) & ~COL_A;
    return diag;
}

static constexpr uint64_t west_shift(uint64_t diag, uint8_t n) {
    for (uint8_t i = 0; i < n; ++i)
        diag = diag >> 1 & ~COL_H;
    return diag;
}


static constexpr std::array<std::array<uint64_t, 8>, 64> calculate_rays() {
    std::array<std::array<uint64_t, 8>, 64> rays{};

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

        rays[i][NORTH] = (COL_A & ~ROW1) << i;
        rays[i][SOUTH] = (COL_H & ~ROW8) >> (63 - i);
        rays[i][EAST]  = east;
        rays[i][WEST]  = west;

        east = east << 1 & ~COL_A;
        west |= one;
        one <<= 1;

        // i % 8 - get col
        // i / 8 - get row
        rays[i][NORTH_EAST] = east_shift(DIAG_A1_H8 & ~ROW1, i % 8)       << (i - (i % 8));
        rays[i][NORTH_WEST] = west_shift(DIAG_A8_H1 & ~ROW1, 7 - (i % 8)) << (i - (i % 8));
        rays[i][SOUTH_EAST] = east_shift(DIAG_A8_H1 & ~ROW8, i % 8)       >> ((7 - i/8) * 8);
        rays[i][SOUTH_WEST] = west_shift(DIAG_A1_H8 & ~ROW8, 7 - (i % 8)) >> ((7 - i/8) * 8);
    }
    return rays;
}

// 8 sides of a chess piece
// Rays[bit][side]
constexpr std::array<std::array<uint64_t, 8>, 64> Rays = calculate_rays();

#endif //CHESSAI_RAYS_HPP
