#ifndef CHESSAI_GLOBAL_HPP
#define CHESSAI_GLOBAL_HPP

#include <cstdint>
#include <iostream> // Maybe need to delete later

using bitboard = uint64_t;

static constexpr uint8_t PAWN   {0};
static constexpr uint8_t ROOK   {1};
static constexpr uint8_t KNIGHT {2};
static constexpr uint8_t BISHOP {3};
static constexpr uint8_t KING   {4};
static constexpr uint8_t QUEEN  {5};

static constexpr uint8_t BLACK  {0};
static constexpr uint8_t WHITE  {1};

// The method for counting is taken from the website: https://www.chessprogramming.org/BitScan
static constexpr uint8_t BitScan[64] = {
        0, 47,  1, 56, 48, 27,  2, 60,
        57, 49, 41, 37, 28, 16,  3, 61,
        54, 58, 35, 52, 50, 42, 21, 44,
        38, 32, 29, 23, 17, 11,  4, 62,
        46, 55, 26, 59, 40, 36, 15, 53,
        34, 51, 20, 43, 31, 22, 10, 45,
        25, 39, 14, 33, 19, 30,  9, 24,
        13, 18,  8, 12,  7,  6,  5, 63
};

/** Arrangements for the presentation of the board (uint64_t) */
/**
 *      A   B   C   D   E   F   G   H
 * 8 | 56  57  58  59  60  61  62  63
 * 7 | 48  49  50  51  52  53  54  55
 * 6 | 40  41  42  43  44  45  46  47
 * 5 | 32  33  34  35  36  37  38  39
 * 4 | 24  25  26  27  28  29  30  31
 * 3 | 16  17  18  19  20  21  22  23
 * 2 |  8   9  10  11  12  13  14  15
 * 1 |  0   1   2   3   4   5   6   7
 */

extern void set0(bitboard& field, uint8_t pos);
extern void set1(bitboard& field, uint8_t pos);
extern bool get_bit(bitboard field, uint8_t pos);
extern uint8_t count1(bitboard field);

// Least/Most Significant Bit
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
extern uint8_t lsb(bitboard bb);
extern uint8_t msb(bitboard bb);

// For test, NEED TO DELETE LATER!!!
void print(bitboard bb);

#endif //CHESSAI_GLOBAL_HPP
