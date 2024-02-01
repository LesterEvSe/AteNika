#include "bitfunc.hpp"
#include "zobrist_hash.hpp"
#include "rays.hpp"
#include "attacks.hpp"
//#include "mvv_lva.hpp"
//#include "pst.hpp"
//#include "eval.hpp"
//#include "uci.hpp"
//#include "pgn_parser.hpp"

#include "board.hpp"
#include "search.hpp"

int main() {
    init_bits_pre_calculation(); // Must be at the beginning!
    ZobristHash::init();
    Rays::init();
    Attacks::init(); // Must be init after Rays

    Board board = Board();
    std::cout << board;
    Search::iter_deep(board);

//    MvvLva::init();
//    PieceTables::init();
//    Eval::init();
//    Search::init();


    // Data take from https://www.ficsgames.org/download.html
    // PGNParser::parse("../ficsgamesdb_2020_CvC_nomovetimes_317502.pgn");
//    Uci::init();
//    Uci::start();
    return 0;
}
