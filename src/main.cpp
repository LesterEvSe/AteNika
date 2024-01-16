#include "rays.hpp"
#include "mvv_lva.hpp"
#include "pst.hpp"
#include "eval.hpp"
#include "board.hpp"
#include "uci.hpp"

int main() {
    init_bits_pre_calculation(); // Must be at the beginning!
    Rays::init();
    Attacks::init(); // Must be init after Rays
    MvvLva::init();
    ZobristHash::init();
    PieceTables::init();
    Eval::init();

    Uci::start();
    return 0;

//    std::thread test(some_func);
//    test.detach();
//
//    std::string command;
//    do {
//        std::cout << "Enter 'exit' if you want exit: ";
//        std::cin >> command;
//    } while (command != "exit");
//
//    stop = true;


//    Board board = Board();
//    std::cout << board;
//    for (int i = 0; i < 1000; ++i) {
//        std::cout << "ply: " << i << std::endl;
//        Search::restart();
////        Search::restart(false, 50);
//        Search::iter_deep(board);
//
//        Move best_move = Search::get_best_move();
//        board.make(best_move);
//        std::cout << board;
//    }

    return 0;
}
