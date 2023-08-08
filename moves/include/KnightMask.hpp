#ifndef CHESSAI_KNIGHTMASK_HPP
#define CHESSAI_KNIGHTMASK_HPP

#include "Bitboard.hpp"

static constexpr std::array<bitboard, 64> calculate_knight_mask() {
    std::array<bitboard, 64> mask{};
    for (int i = 0; i < 64; ++i)
    {
        bitboard bit = ONE << i;

        // lr - left and right shift
        bitboard lr1 = (bit >> 1) & ~COL_H | // left
                       (bit << 1) & ~COL_A;  // right
        bitboard lr2 = (bit >> 2) & ~(COL_G | COL_H) |
                       (bit << 2) & ~(COL_A | COL_B);
        mask[i] = lr1 << 16 | lr1 >> 16 | lr2 << 8 | lr2 >> 8;
    }
    return mask;
}

constexpr std::array<bitboard, 64> KnightMask = calculate_knight_mask();

#endif //CHESSAI_KNIGHTMASK_HPP
