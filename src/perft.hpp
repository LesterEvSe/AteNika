#ifndef ATENIKA_PERFT_HPP
#define ATENIKA_PERFT_HPP

#include "board.hpp"

// Perft - PERFormance Test, move path enumeration.
// Counts every leaf node at the given depth without evaluating or pruning
// anything. The result is a checksum of the move generator: correct engines all
// agree on it, so a mismatch means a movegen bug and nothing else.
// https://www.chessprogramming.org/Perft_Results
namespace Perft {

    int64_t run(Board &board, int depth);

    // Same total, but printed one line per root move:
    //
    //     a2a3: 2186
    //     b2b3: 1964
    //     ...
    //
    //     Nodes searched: 97862
    //
    // The format matches Stockfish's `go perft`, so the two can be diffed
    // directly to find which root move disagrees, then recursed into.
    int64_t divide(Board &board, int depth);

} // Perft

#endif //ATENIKA_PERFT_HPP
