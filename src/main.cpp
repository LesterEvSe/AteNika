#include "rays.hpp"
#include "mvv_lva.hpp"
#include "pst.hpp"
#include "eval.hpp"
#include "search.hpp"

int main() {
    init_bits_pre_calculation(); // Must be at the beginning!
    Rays::init();
    Attacks::init(); // Must be init after Rays
    MvvLva::init();
    ZobristHash::init();
    PieceTables::init();
    Eval::init();

    return 0;
}
