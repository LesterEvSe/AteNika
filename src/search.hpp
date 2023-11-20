#ifndef ATENICA_SEARCH_HPP
#define ATENICA_SEARCH_HPP

#include "board.hpp"
#include "history.hpp"
// #include <unordered_set>
#include <chrono>

namespace Search {
namespace hidden {
    Move _best_move;

    // TODO Test unordered_set, history with 50 and 51 bits
    // last 50 moves
    // Maybe more efficiently use static C array with search.
    // Create custom class for this case
    // std::unordered_set<bits96> _history;
    History _history = History();

    /* Need to implement iter Deep (as root method)
     * negamax or minimax algo
     * use limits and testing, MORE TESTING!
     * I want to search on deep 20
     */


} // hidden
    void iter_deep();
    void get_best_move();
    void stop(); // stop search if time has expired
}

#endif //ATENICA_SEARCH_HPP
