#include "global.hpp"

bitboard g_rows[DIM];
bitboard g_invert_rows[DIM];

bitboard g_cols[DIM];
bitboard g_invert_cols[DIM];

// For test, NEED TO DELETE LATER!!!
void print(bitboard bb) {
    std::cout << "   ";

    for (char let = 'A'; let <= 'H'; ++let)
        std::cout << ' ' << let;
    std::cout << std::endl;

    for (int i = 0; i < 8; ++i) {
        std::cout << i+1 << " |";
        for (int j = 0; j < 8; ++j)
            // equivalent to bb[8*i + j]
            std::cout << ' ' << bool(bb & (bitboard(1) << ((i << 3) + j)));

        std::cout << std::endl;
    }
    std::cout << "\n--------------------\n\n";
}

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
}