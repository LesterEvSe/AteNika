#ifndef CHESSAI_NAMES_HPP
#define CHESSAI_NAMES_HPP

#include <cstdint>

using bitboard = uint64_t;
static constexpr uint8_t DIM = 8;

bitboard g_rows[DIM];
bitboard g_invert_rows[DIM];

bitboard g_cols[DIM];
bitboard g_invert_cols[DIM];

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

// NEED TO DELETE LATER!!!
void print(bitboard bb);

void set_rows_cols() {
    bitboard row = 0, col = 0;
    for (int i = 0; i < DIM; ++i) {
        row |= bitboard(1) << i;
        col |= bitboard(1) << (i * DIM);
    }

    for (int i = 0; i < DIM; ++i) {
        g_rows[i] = row;
        g_invert_rows[i] = ~row;

        g_cols[i] = col;
        g_invert_cols[i] = ~col;

        row <<= DIM;
        col <<= 1;
    }

    for (int i = 0; i < DIM; ++i) {
        print(g_rows[i]);
        print(g_cols[i]);
    }
}

const bool BLACK = false;
const bool WHITE = true;

#endif //CHESSAI_NAMES_HPP
