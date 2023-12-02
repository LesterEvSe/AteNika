#include "rays.hpp"
#include "board.hpp"
#include "movepicker.hpp"
#include "pst.hpp"
#include "eval.hpp"
#include "search.hpp"

int main() {
    init_bits_pre_calculation(); // Must be at the beginning!
    Rays::init();
    Attacks::init(); // Must be init after Rays
    MovePicker::init();
    ZobristHash::init();
    PieceTables::init();
    Eval::init();

    return 0;
}
