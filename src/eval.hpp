#ifndef ATENIKA_EVAL_HPP
#define ATENIKA_EVAL_HPP

#include "board.hpp"

namespace Eval {
namespace hidden {

    // Alpha Zero Material Bonus
    constexpr int32_t MATERIAL_BONUS[PIECE_SIZE] {
        100, 305, 333, 563, 950, 0
    };

    int32_t eval_material(const Board &board, Color color);

} // hidden

    int32_t evaluate(const Board &board);
} // Eval

#endif //ATENIKA_EVAL_HPP
