#ifndef ATENIKA_EVAL_HPP
#define ATENIKA_EVAL_HPP

#include "board.hpp"

// Values and ideas for evaluations takes from https://hxim.github.io/Stockfish-Evaluation-Guide/
// Eval - https://www.chessprogramming.org/Evaluation_Function

namespace Eval {
namespace hidden {

    constexpr int32_t MATERIAL_BONUS[PHASES][PIECE_SIZE] {
        {124, 781, 825, 1276, 2538},
        {206, 854, 915, 1380, 2682}
    };

    // Second number is number of attacks of a specific piece
    constexpr int32_t KNIGHT_MOBILITY_BONUS[PHASES][9] {
            {-62,-53,-12,-4,3,13,22,28, 33},
            {-81,-56,-31,-16,5,11,17,20, 25}
    };
    constexpr int32_t BISHOP_MOBILITY_BONUS[PHASES][14] {
            {-48,-20,16,26,38,51,55,63,63,68,81,81,91,98},
            {-59,-23,-3,13,24,42,54,57,65,73,78,86,88,97}
    };
    constexpr int32_t ROOK_MOBILITY_BONUS[PHASES][15] {
            {-60,-20,2,3,3,11,22,31,40,40,41,48,57,57,62},
            {-78,-17,23,39,70,99,103,121,134,139,158,164,168,169,172}
    };
    constexpr int32_t QUEEN_MOBILITY_BONUS[PHASES][28] {
            {-30,-12,-8,-9,20,23,23,35,38,53,64,65,65,66,
             67,67,72,72,77,79,93,108,108,108,110,114,114,116},
            {-48,-30,-7,19,40,55,59,75,78,96,96,100,121,127,
             131,133,136,141,147,150,151,168,168,171,182,182,192,219}
    };

    template<GamePhase phase, Color color, bool with_pawns> int32_t _eval_material(const Board &board);
    template<GamePhase phase, Color color> int32_t _eval_mobility(const Board &board);

    int32_t _tapered_eval(const Board &board);

    template<GamePhase phase> int32_t _eval_for_phase(const Board &board);

} // hidden

    int32_t evaluate(const Board &board);
} // Eval

#endif //ATENIKA_EVAL_HPP
