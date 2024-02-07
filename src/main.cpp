#include "bitfunc.hpp"
#include "zobrist_hash.hpp"
#include "rays.hpp"
#include "attacks.hpp"
#include "mvv_lva.hpp"
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
    MvvLva::init();
    Search::init();

    Board board = Board("r1b1k2r/ppppnppp/2n2q2/2b5/3Np3/2P1B3/PP3PPP/RN1QKB1R w Kqkq - 0");
    board.display_all();
    Search::iter_deep(board, true);
    std::cout << (std::string)Search::get_best_move();

//    PieceTables::init();
//    Eval::init();

    // Data take from https://www.ficsgames.org/download.html
    // PGNParser::parse("../ficsgamesdb_2020_CvC_nomovetimes_317502.pgn");
//    Uci::init();
//    Uci::start();
    return 0;
}
