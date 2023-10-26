#include "rays.hpp"
#include "board.hpp"
#include "movepicker.hpp"
#include "piece_square_tables.hpp"

int main() {
    Rays::init();
    Attacks::init(); // Must be init after Rays
    MovePicker::init();
    ZobristHash::init();
    PieceTables::init();

    return 0;
}
