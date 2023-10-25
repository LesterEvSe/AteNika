#ifndef ATENICA_EVALUATION_HPP
#define ATENICA_EVALUATION_HPP

#include "board.hpp"

namespace Evaluation {
namespace hidden {
    const int32_t MATERIAL_VALUES[PHASES][PIECE_SIZE] = {
        // Alpha Zero evaluation for classical position
        [OPENING] = {
            [PAWN] = 100,
            [KNIGHT] = 305,
            [BISHOP] = 333,
            [ROOK] = 563,
            [QUEEN] = 950,
            [KING] = 50'000,
        },
        [ENDGAME] = {
            [PAWN] = 120,
            [KNIGHT] = 300,
            [BISHOP] = 310,
            [ROOK] = 500,
            [QUEEN] = 960,
            [KING] = 50'000,
        }
    };

    int32_t has_bishop_pair(const Board &board, Color color); // +45 op, +55 end
    int32_t rook_on_open_file(const Board &board, Color color); // +20 on opening, skip at endgame
    int32_t doubled_rooks(const Board &board, Color color); // +15
    int32_t color_weakness(const Board &board, Color color); // If fields of one color are worse controlled than for another color
    int32_t lost_castling(const Board &board, Color color); // -50

    // Pawn evaluates
    int32_t isolated_pawns(const Board &board, Color color); // -10
    int32_t connected_pawns(const Board &board, Color color); // Pawn protected by another pawn +12
    int32_t pawn_islands(const Board &board, Color color); // Maybe need to delete
    int32_t doubled_pawns(const Board &board, Color color); // -25
    int32_t passed_pawns(const Board &board, Color color);
    int32_t king_pawns_shield(const Board &board, Color color);

}; // hidden

    int32_t evaluate(const Board &board, Color color);
};

#endif //ATENICA_EVALUATION_HPP
