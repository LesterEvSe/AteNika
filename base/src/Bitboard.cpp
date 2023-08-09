#include "Bitboard.hpp"
#include <bit> // for std::popcount

void set0(bitboard& field, uint8_t pos)   { field &= ~(bitboard(1) << pos);      }
void set1(bitboard& field, uint8_t pos)   { field |= bitboard(1) << pos;         }
bool get_bit(bitboard field, uint8_t pos) { return (field & bitboard(1) << pos); }
uint8_t count1(bitboard field)            { return std::popcount(field);      }

uint8_t lsb(bitboard bb) { return BitScan[((bb ^ (bb - 1)) * bitboard(0x03f79d71b4cb0a89)) >> 58]; }
uint8_t msb(bitboard bb) {
    bb |= bb >> 1;
    bb |= bb >> 2;
    bb |= bb >> 4;
    bb |= bb >> 8;
    bb |= bb >> 16;
    bb |= bb >> 32;
    return BitScan[(bb * bitboard(0x03f79d71b4cb0a89)) >> 58];
}

// For test, NEED TO DELETE LATER
void printbb(bitboard bb) {
    std::cout << "   ";

    for (char let = 'A'; let <= 'H'; ++let)
        std::cout << ' ' << let;
    std::cout << std::endl;

    for (int8_t i = 7; i >= 0; --i) {
        std::cout << i+1 << " |";
        for (uint8_t j = 0; j < 8; ++j)
            std::cout << ' ' << bool(bb & (bitboard(1) << (i*8 + j)));

        std::cout << std::endl;
    }
    std::cout << "\n--------------------\n\n";
}