#include <iostream>
#include "pieces.hpp"

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


/** Hm.. Maybe switch to 'C' language..?
 *  About 18 bitboards, I can realize them with struct or class with methods*/
int main() {
    set_rows_cols();
    bitboard temp = 123'512;

//    Piece::set0(temp, 4);
//    board(temp);
//    std::cout << unsigned(Piece::bsf(temp)) << ' ' << unsigned(Piece::bsr(temp));

    return 0;
}
