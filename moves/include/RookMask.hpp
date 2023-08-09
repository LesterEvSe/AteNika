#ifndef CHESSAI_ROOKMASK_HPP
#define CHESSAI_ROOKMASK_HPP

#include "Rays.hpp"

static constexpr std::array<bitboard, 64> calculate_rook_mask() {
    std::array<bitboard, 64> mask{};
    for (uint8_t i = 0; i < 64; ++i) {
        mask[i] = (Rays[i][NORTH] & ~ROW8) |
                  (Rays[i][EAST] & ~COL_H) |
                  (Rays[i][SOUTH] & ~ROW1) |
                  (Rays[i][WEST] & ~COL_A);
    }
    return mask;
}

constexpr std::array<bitboard, 64> RookMask = calculate_rook_mask();

#endif //CHESSAI_ROOKMASK_HPP
