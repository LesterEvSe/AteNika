#ifndef CHESSAI_BITBOARD_HPP
#define CHESSAI_BITBOARD_HPP

#include <cstdint>
#include <array>
#include <iostream> // Maybe need to delete later

using bitboard = uint64_t;

constexpr bitboard colA = 0x0101010101010101;
constexpr bitboard colB = colA << 1;
constexpr bitboard colC = colA << 2;
constexpr bitboard colD = colA << 3;
constexpr bitboard colE = colA << 4;
constexpr bitboard colF = colA << 5;
constexpr bitboard colG = colA << 6;
constexpr bitboard colH = colA << 7;

constexpr bitboard line1 = 0xFF;
constexpr bitboard line2 = line1 << (8 * 1);
constexpr bitboard line3 = line1 << (8 * 2);
constexpr bitboard line4 = line1 << (8 * 3);
constexpr bitboard line5 = line1 << (8 * 4);
constexpr bitboard line6 = line1 << (8 * 5);
constexpr bitboard line7 = line1 << (8 * 6);
constexpr bitboard line8 = line1 << (8 * 7);

constexpr uint8_t PAWN   {0};
constexpr uint8_t ROOK   {1};
constexpr uint8_t KNIGHT {2};
constexpr uint8_t BISHOP {3};
constexpr uint8_t KING   {4};
constexpr uint8_t QUEEN  {5};

constexpr uint8_t BLACK  {0};
constexpr uint8_t WHITE  {1};

// The method for counting is taken from the website: https://www.chessprogramming.org/BitScan
constexpr std::array<uint8_t, 64> BitScan {
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

/**
       A B C D E F G H
   8 | r n b q k b n r
   7 | p p p p p p p p
   6 | - - - - - - - -
   5 | - - - - - - - -
   4 | - - - - - - - -
   3 | - - - - - - - -
   2 | P P P P P P P P
   1 | R N B Q K B N R
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
void printbb(bitboard bb);

#endif //CHESSAI_BITBOARD_HPP
