#include <iostream>
#include "pieces.hpp"

// For test, NEED TO DELETE LATER!!!
void board(bitboard val) {
    std::bitset<64> bits(val);
    std::cout << "   ";

    for (char let = 'A'; let <= 'H'; ++let)
        std::cout << ' ' << let;
    std::cout << std::endl;

    for (int i = 0; i < 8; ++i) {
        std::cout << i+1 << " |";
        for (int j = 0; j < 8; ++j)
            std::cout << ' ' << bits[8*i + j];
        std::cout << std::endl;
    }
    std::cout << "\n--------------------\n\n";
}

int main() {
    Pawn white_pawn = Pawn(WHITE);
    Pawn black_pawn = Pawn(BLACK);

    Knight black_knight = Knight(BLACK);
    Knight white_knight = Knight(WHITE);

    King black_king = King(BLACK);
    King white_king = King(WHITE);

    board(white_king.get_attack());
    board(black_king.get_attack());

    return 0;
}
