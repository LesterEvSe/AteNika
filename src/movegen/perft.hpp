#ifndef ATENIKA_PERFT_HPP
#define ATENIKA_PERFT_HPP

#include "core/board.hpp"

// https://www.chessprogramming.org/Perft_Results
namespace Perft {

  int64_t run(Board &board, int depth);

  // The format matches Stockfish's `go perft`, so the two can be diffed
  // directly to find which root move disagrees, then recursed into.
  int64_t divide(Board &board, int depth);

} // namespace Perft

#endif // ATENIKA_PERFT_HPP
