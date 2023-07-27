#include "global.hpp"

// For test, NEED TO DELETE LATER!!!
void print(bitboard bb) {
    std::cout << "   ";

    for (char let = 'A'; let <= 'H'; ++let)
        std::cout << ' ' << let;
    std::cout << std::endl;

    for (int8_t i = 7; i >= 0; --i) {
        std::cout << i+1 << " |";
        for (uint8_t j = 0; j < 8; ++j)
            // equivalent to bb[8*i + j]
            std::cout << ' ' << bool(bb & (bitboard(1) << ((i << 3) + j)));

        std::cout << std::endl;
    }
    std::cout << "\n--------------------\n\n";
}
