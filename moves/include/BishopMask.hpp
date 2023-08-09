#ifndef CHESSAI_BISHOPMASK_HPP
#define CHESSAI_BISHOPMASK_HPP

#include "Rays.hpp"

static constexpr std::array<bitboard, 64> calculate_bishop_mask() {
    uint64_t invert_edges_mask = ~(ROW1 | ROW8 | COL_A | COL_H);
    std::array<bitboard, 64> mask{};

    for (uint8_t i = 0; i < 64; ++i) {
        mask[i] = (Rays[i][NORTH_EAST] | Rays[i][SOUTH_EAST] |
                   Rays[i][SOUTH_WEST] | Rays[i][NORTH_WEST]) & invert_edges_mask;
    }
    return mask;
}

constexpr std::array<bitboard, 64> BishopMask = calculate_bishop_mask();

#endif //CHESSAI_BISHOPMASK_HPP
