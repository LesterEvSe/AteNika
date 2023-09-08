#include "rays.hpp"
#include "zobrist_hash.hpp"
#include "board.hpp"

int main() {
    Rays::init();
    ZobristHash::init();

    Board board = Board();
    std::cout << 0b1101;
    return 0;
}
