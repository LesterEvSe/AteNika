#include "rays.hpp"
#include "mvv_lva.hpp"
#include "pst.hpp"
#include "eval.hpp"
#include "board.hpp"
#include "uci.hpp"

int main() {
    init_bits_pre_calculation(); // Must be at the beginning!
    ZobristHash::init();
    Rays::init();
    Attacks::init(); // Must be init after Rays
    MvvLva::init();
    PieceTables::init();
    Eval::init();
    Search::init();

    Uci::init();
    Uci::start();
    return 0;
}
