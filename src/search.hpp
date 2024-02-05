#ifndef ATENIKA_SEARCH_HPP
#define ATENIKA_SEARCH_HPP

#include "move.hpp"

#include <chrono>

namespace Search {
namespace hidden {
    extern Move _best_move;

    extern int16_t _depth;
    extern int64_t _nodes;
    extern std::chrono::time_point<std::chrono::steady_clock> _start;

    void _debug(const Board &board, int depth, int best_score, int elapsed);
    int32_t _negamax(Board &board, int16_t ply, int16_t depth, int32_t alpha, int32_t beta);

} // hidden

    void init();
    void iter_deep(Board &board, bool debug);
} // Search

#endif //ATENIKA_SEARCH_HPP
