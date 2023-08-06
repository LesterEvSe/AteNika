#ifndef CHESSAI_KING_MASK_HPP
#define CHESSAI_KING_MASK_HPP

#include "Bitboard.hpp"
#include <array>

static constexpr std::array<bitboard, 64> calculate_king_mask() {
    std::array<bitboard, 64> mask{1};
    for (uint8_t i = 0; i < 64; ++i) {
    }
    return mask;
}
constexpr std::array<bitboard, 64> KingMask = calculate_king_mask();

#endif //CHESSAI_KING_MASK_HPP
