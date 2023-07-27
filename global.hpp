#ifndef CHESSAI_GLOBAL_HPP
#define CHESSAI_GLOBAL_HPP

#include <cstdint>
#include <iostream> // Maybe need to delete later

using bitboard = uint64_t;

/** Arrangements for the presentation of the board (unsigned long long) */
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

// For test, NEED TO DELETE LATER!!!
void print(bitboard bb);

#endif //CHESSAI_GLOBAL_HPP
