#ifndef ATENIKA_BITFUNC_HPP
#define ATENIKA_BITFUNC_HPP

#include "defs.hpp"
#include <iostream> // for std::cerr
#include <string>

inline uint8_t get_rank(uint8_t cell) { return cell >> 3; }
inline uint8_t get_file(uint8_t cell) { return cell & 7; }
inline Color get_opposite_move(Color color) { return color == WHITE ? BLACK : WHITE; }

constexpr uint16_t MAX_VAL = 0xFFFF;
extern uint8_t memo_bits[1 << 16];
void init_bits_pre_calculation();

//inline uint8_t count_bits(uint64_t field) { return std::popcount(field); }
inline uint8_t count_bits(uint64_t board) {
    return memo_bits[board & MAX_VAL] + memo_bits[(board >> 16) & MAX_VAL] +
        memo_bits[(board >> 32) & MAX_VAL] + memo_bits[(board >> 48) & MAX_VAL];
}
inline void set(bitboard &bb, uint8_t cell)   { bb |= ONE << cell; }
inline void reset(bitboard &bb, uint8_t cell) { bb &= ~(ONE << cell); }

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
inline uint8_t lsb(bitboard bb) { return BitScan[((bb ^ (bb - 1)) * 0x03f79d71b4cb0a89ull) >> 58]; }
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
    bb &= bb - 1;
    return pos;
}

inline uint8_t get_cell(std::string notation) {
    uint8_t file = notation[0] - 'a';
    uint8_t rank = notation[1] - '1';
    return (rank << 3) + file;
}

inline void error(const std::string &msg) {
    std::cerr << msg;
    exit(1);
}

// For debugging action
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

#endif //ATENIKA_BITFUNC_HPP
