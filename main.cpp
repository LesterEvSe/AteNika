#include "rays.hpp"
#include "attacks.hpp"
#include "zobrist_hash.hpp"
#include "board.hpp"

int main() {
    uint64_t mask = 6112432;
    printbb(mask);
    uint8_t pos = lsb(mask);
    mask &= ~(ONE << pos);
    mask

    printbb(mask);

    Rays::init();
    Attacks::init(); // Must be init after Rays
    ZobristHash::init();

    return 0;
}
