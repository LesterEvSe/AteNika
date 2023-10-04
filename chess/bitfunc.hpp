#ifndef CHESSAI_BITFUNC_HPP
#define CHESSAI_BITFUNC_HPP

#include "defs.hpp"
#include <iostream> // for std::cerr
#include <string>
#include <bit> // for std::popcount

inline uint8_t get_row(uint8_t cell) { return cell >> 3; }
inline uint8_t get_col(uint8_t cell) { return cell & 7; }
inline Color get_opposite_move(Color color) { return color == WHITE ? BLACK : WHITE; }

inline uint8_t count_bits(uint64_t field) { return std::popcount(field); }
inline void set0(bitboard &bb, uint8_t cell) { bb &= ~(ONE << cell); }
inline void set1(bitboard &bb, uint8_t cell) { bb |= ONE << cell; }

/**
 * bitScanForward/LSB
 * @author Kim Walisch (2012)
 * @param bb bitboard to scan
 * @return index (0..63) of least significant one bit
 */
/**
 * bitScanReverse/MSB
 * @authors Kim Walisch, Mark Dickinson
 * @param bb bitboard to scan
 * @return index (0..63) of most significant one bit
 */

// The method for counting is taken from the website: https://www.chessprogramming.org/BitScan
static constexpr uint8_t BitScan[64] {
        0,  47,  1, 56, 48, 27,  2, 60,
        57, 49, 41, 37, 28, 16,  3, 61,
        54, 58, 35, 52, 50, 42, 21, 44,
        38, 32, 29, 23, 17, 11,  4, 62,
        46, 55, 26, 59, 40, 36, 15, 53,
        34, 51, 20, 43, 31, 22, 10, 45,
        25, 39, 14, 33, 19, 30,  9, 24,
        13, 18,  8, 12,  7,  6,  5, 63
};

// IMPORTANT NOTE! lsb faster than msb!!!
inline uint8_t lsb(bitboard bb) { return BitScan[((bb ^ (bb - 1)) * bitboard(0x03f79d71b4cb0a89)) >> 58]; }
inline uint8_t msb(bitboard bb) {
    bb |= bb >> 1;
    bb |= bb >> 2;
    bb |= bb >> 4;
    bb |= bb >> 8;
    bb |= bb >> 16;
    bb |= bb >> 32;
    return BitScan[(bb * bitboard(0x03f79d71b4cb0a89)) >> 58];
}

inline uint8_t pop_lsb(bitboard &bb) {
    uint8_t pos = lsb(bb);
    set0(bb, pos);
    return pos;
}

inline void error(const std::string &msg) {
    std::cerr << msg;
    exit(1);
}

inline void printbb(bitboard bb) {
    std::cout << "   ";

    for (char let = 'A'; let <= 'H'; ++let)
        std::cout << ' ' << let;
    std::cout << std::endl;

    for (int8_t i = 7; i >= 0; --i) {
        std::cout << i+1 << " |";
        for (uint8_t j = 0; j < 8; ++j)
            std::cout << ' ' << bool(bb & (ONE << (i*8 + j)));
        std::cout << std::endl;
    }
    std::cout << "\n--------------------\n\n";
}

#endif //CHESSAI_BITFUNC_HPP
