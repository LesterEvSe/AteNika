#ifndef ATENICA_SEARCH_HPP
#define ATENICA_SEARCH_HPP

#include "board.hpp"
#include "history.hpp"
#include <limits>

namespace Search {
namespace hidden {

    extern Move _best_move;
    extern int32_t _best_score;

    extern int64_t _nodes;

    // TODO Test unordered_set, history with 50 and 51 bits
    // last 50 moves
    // Maybe more efficiently use static C array with search.
    // Create custom class for this case
    // std::unordered_set<bits96> _history;
    extern History _history;

    /* Need to implement iter Deep (as root method)
     * negamax or minimax algo
     * use limits and testing, MORE TESTING!
     * I want to search on deep 20
     */

    extern bool _stop; // if time expired, then false
    constexpr int16_t MAX_DEPTH = 7; // TODO want depth = 20
//    constexpr int32_t INF = std::numeric_limits<int32_t>::max();

    int32_t _negamax(const Board &board, int16_t depth, int32_t alpha, int32_t beta);
    int32_t _quiescence_search(const Board &board, int32_t alpha, int32_t beta);

} // hidden
    void restart();
    void stop(); // stop search if time has expired

    Move get_best_move();
    void iter_deep(const Board &board);
}

#endif //ATENICA_SEARCH_HPP
