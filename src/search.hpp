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
    extern History _history;

    extern int32_t _time_allocated_ms;
    extern bool _without_time;
    extern bool _stop; // if time expired, then false

    // Curr MAX_DEPTH = 7. Not bigger, or calculate more than 1 minute
    constexpr int16_t MAX_DEPTH = 7; // Want depth = 20. Unfortunately too much

    int32_t _negamax(const Board &board, int16_t depth, int32_t alpha, int32_t beta);
    int32_t _quiescence_search(const Board &board, int32_t alpha, int32_t beta);

} // hidden
    void restart(bool without_time = true, int32_t time_allocated_ms = 0);
    void stop(); // stop search if time has expired

    Move get_best_move();
    void iter_deep(const Board &board);
}

#endif //ATENICA_SEARCH_HPP
