#ifndef ATENIKA_EVAL_HPP
#define ATENIKA_EVAL_HPP

#include "board.hpp"

// Values and ideas for evaluations takes from https://hxim.github.io/Stockfish-Evaluation-Guide/
// Eval - https://www.chessprogramming.org/Evaluation_Function

namespace Eval {
namespace hidden {

    constexpr int32_t MATERIAL_BONUS[PIECE_SIZE] {
        100, 320, 330, 500, 900, 0
    };

    template<Color color> int32_t _eval_material(const Board &board);

} // hidden

    int32_t evaluate(const Board &board);
} // Eval

#endif //ATENIKA_EVAL_HPP
