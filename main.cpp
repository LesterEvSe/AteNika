#include "rays.hpp"
#include "attacks.hpp"
#include "zobrist_hash.hpp"
#include "board.hpp"

int main() {
    Rays::init();
    Attacks::init(); // Must be init after Rays
    ZobristHash::init();

    return 0;
}
