#include "rays.hpp"
#include "board.hpp"
#include "movegen.hpp"

int main() {
    Rays::init();
    Attacks::init(); // Must be init after Rays
    MovePicker::init();
    ZobristHash::init();

    Board board = Board();
    std::cout << board;

    return 0;
}
