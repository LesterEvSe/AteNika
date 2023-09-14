#include "rays.hpp"
#include "attacks.hpp"
#include "zobrist_hash.hpp"
#include "board.hpp"

int main() {
    Rays::init();
    Attacks::init(); // Must be init after Rays
    ZobristHash::init();

    bitboard temp = 0x0004120843280816;
    uint8_t cell = 28; // 4E

    printbb(temp);
    printbb(Attacks::get_queen_attacks(cell, temp)); // Accept

    return 0;
}
