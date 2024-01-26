#include "rays.hpp"
#include "mvv_lva.hpp"
#include "pst.hpp"
#include "eval.hpp"
#include "board.hpp"
#include "uci.hpp"
#include "pgn_parser.hpp"

int main() {
    init_bits_pre_calculation(); // Must be at the beginning!
    ZobristHash::init();
    Rays::init();
    Attacks::init(); // Must be init after Rays
    MvvLva::init();
    PieceTables::init();
    Eval::init();
    Search::init();

    // Data take from https://www.ficsgames.org/download.html
    // PGNParser::parse("../ficsgamesdb_2020_CvC_nomovetimes_317393.pgn");
    Uci::init("../book.txt");
    Uci::start();
    return 0;
}
