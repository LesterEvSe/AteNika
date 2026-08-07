#include "bitboard/attacks.hpp"
#include "bitboard/bitfunc.hpp"
#include "bitboard/rays.hpp"
#include "core/zobrist_hash.hpp"
#include "eval/eval.hpp"
#include "search/mvv_lva.hpp"
#include "search/search.hpp"
#include "uci/interface.hpp"

int main() {
  init_bits_pre_calculation(); // Must be at the beginning!
  ZobristHash::init();
  Rays::init();
  Attacks::init(); // Must be init after Rays
  MvvLva::init();
  Eval::init();
  Search::init();

  // Data take from https://www.ficsgames.org/download.html
  Uci::start();
  return 0;
}
