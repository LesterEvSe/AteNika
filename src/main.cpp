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
    Search::init();

    // TODO Deal with print PV nodes, PV search and similar stuff
    Board board = Board("2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0");
//    Board board = Board("2rr3k/pp3pp1/1nnqbNQp/3pN3/2pP4/2P5/PPB4P/R4RK1 w - - 0");
    board.display_all();
    Search::iter_deep(board, true);

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
