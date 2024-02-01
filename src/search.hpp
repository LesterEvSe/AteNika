#ifndef ATENIKA_SEARCH_HPP
#define ATENIKA_SEARCH_HPP

#include "move.hpp"

namespace Search {
namespace hidden {
    extern Move _best_move;
    extern int32_t _best_score;

    extern int32_t _nodes;

    int32_t _negamax(Board &board, uint16_t depth, int32_t alpha, int32_t beta);

} // hidden

    void iter_deep(Board &board);
} // Search

#endif //ATENIKA_SEARCH_HPP
