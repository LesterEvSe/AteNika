#include <iostream>
#include "pieces.hpp"

int main() {
    Pawn white_pawn = Pawn(WHITE);
    Pawn black_pawn = Pawn(BLACK);

    Knight black_knight = Knight(BLACK);
    Knight white_knight = Knight(WHITE);

    std::cout << white_knight << black_knight;

    return 0;
}
