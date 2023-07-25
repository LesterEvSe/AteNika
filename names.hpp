#ifndef CHESSAI_NAMES_HPP
#define CHESSAI_NAMES_HPP

#include <cstdint>

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

// For example column C.
// For the rows respectively also
/*
 * 00100000
 * 00100000
 * 00100000
 * 00100000
 * 00100000
 * 00100000
 * 00100000
 * 00100000
 */

const bitboard ROW8 = 18374686479671623680;
const bitboard ROW7 = 71776119061217280;
const bitboard ROW6 = 280375465082880;
const bitboard ROW5 = 1095216660480;
const bitboard ROW4 = 4278190080;
const bitboard ROW3 = 16711680;
const bitboard ROW2 = 65280;
const bitboard ROW1 = 255;

namespace Column {
    const bitboard A = 72340172838076673;
    const bitboard B = 144680345676153346;
    const bitboard C = 289360691352306692;
    const bitboard D = 578721382704613384;
    const bitboard E = 1157442765409226768;
    const bitboard F = 2314885530818453536;
    const bitboard G = 4629771061636907072;
    const bitboard H = 9259542123273814144;
}

const bool BLACK = false;
const bool WHITE = true;

#endif //CHESSAI_NAMES_HPP
