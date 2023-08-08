#ifndef CHESSAI_KINGMASK_HPP
#define CHESSAI_KINGMASK_HPP

#include "Bitboard.hpp"

static constexpr std::array<bitboard, 64> calculate_king_mask() {
    std::array<bitboard, 64> mask{};
    for (uint8_t i = 0; i < 64; ++i)
    {
        bitboard bit = bitboard(1) << i;
        mask[i] = (bit << 7 | bit >> 1 | bit >> 9) & ~COL_H |
                  (bit << 8 | bit >> 8) |
                  (bit << 9 | bit << 1 | bit >> 7) & ~COL_A;
    }
    return mask;
}
constexpr std::array<bitboard, 64> KingMask = calculate_king_mask();

#endif //CHESSAI_KINGMASK_HPP
