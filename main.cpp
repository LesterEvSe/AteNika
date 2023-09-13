#include "rays.hpp"
#include "attacks.hpp"
#include "zobrist_hash.hpp"
#include "board.hpp"

int main() {
    Rays::init();
    Attacks::init(); // Must be init after Rays
    ZobristHash::init();

    uint64_t mask = 6112432;
    printbb(mask);

    for (uint8_t i = 0; i < 64; ++i) {
        std::cout << (int)i << std::endl;
        printbb(Attacks::get_non_sliding_attacks(WHITE, KNIGHT, i));
    }

    return 0;
}
