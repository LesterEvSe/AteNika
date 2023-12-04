#include "rays.hpp"
#include "mvv_lva.hpp"
#include "pst.hpp"
#include "eval.hpp"
#include "search.hpp"
#include "board.hpp"

int main() {
    init_bits_pre_calculation(); // Must be at the beginning!
    Rays::init();
    Attacks::init(); // Must be init after Rays
    MvvLva::init();
    ZobristHash::init();
    PieceTables::init();
    Eval::init();

    Search::restart();

    Board board = Board();
    std::cout << board;
    Search::iter_deep(board);

    Move best_move = Search::get_best_move();
    board.make(best_move);
    std::cout << board;

    return 0;
}
