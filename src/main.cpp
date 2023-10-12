#include "rays.hpp"
#include "board.hpp"
#include "movepicker.hpp"

int main() {
    Rays::init();
    Attacks::init(); // Must be init after Rays
    MovePicker::init();
    ZobristHash::init();

    return 0;
}
