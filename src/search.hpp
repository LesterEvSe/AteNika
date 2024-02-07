#ifndef ATENIKA_SEARCH_HPP
#define ATENIKA_SEARCH_HPP

#include "order_info.hpp"
#include <chrono>

namespace Search {
namespace hidden {

    // Limits
    extern int64_t _nodes;
    extern int32_t _ms_allocated;
    extern int16_t _depth;
    extern bool _stop;

    extern int64_t _fh; // cut-off at n move. The moves are accumulating
    extern int64_t _fhf; // cut-off at first move

    // Search
    extern OrderInfo _order_info;
    extern Move _best_move;
    extern int32_t _best_score;
    extern std::chrono::time_point<std::chrono::steady_clock> _start;

    extern std::string _mate; // for mate check

    void _debug(const Board &board, int depth, int elapsed);
    void _restart();
    int32_t _negamax(Board &board, int16_t depth, int32_t alpha, int32_t beta);

} // hidden

    void init();
    std::string get_mate();
    Move get_best_move();

    bool set_depth(int16_t depth);

    void iter_deep(Board &board, bool debug);
} // Search

#endif //ATENIKA_SEARCH_HPP
