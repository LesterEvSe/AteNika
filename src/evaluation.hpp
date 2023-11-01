#ifndef ATENICA_EVALUATION_HPP
#define ATENICA_EVALUATION_HPP

#include "board.hpp"

namespace Evaluation {
namespace hidden {
    constexpr int32_t MATERIAL_BONUS[PHASES][PIECE_SIZE] = {
        // Alpha Zero evaluation for classical position in OPENING
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

    constexpr int16_t MOBILITY_BONUS[PHASES][PIECE_SIZE] = {
        [OPENING] = {
            [PAWN] = 0,
            [KNIGHT] = 0,
            [BISHOP] = 4,
            [ROOK] = 3,
            [QUEEN] = 1,
            [KING] = 0,
        },
        [ENDGAME] = {
            [PAWN] = 1,
            [KNIGHT] = 1,
            [BISHOP] = 5,
            [ROOK] = 2,
            [QUEEN] = 1,
            [KING] = 1,
        }
    };

    constexpr bitboard NULL_FILES_AND_NEIGHBORS[8] = {
        ~FILE_A & ~FILE_B,
        ~FILE_A & ~FILE_B & ~FILE_C,
        ~FILE_B & ~FILE_C & ~FILE_D,
        ~FILE_C & ~FILE_D & ~FILE_E,
        ~FILE_D & ~FILE_E & ~FILE_F,
        ~FILE_E & ~FILE_F & ~FILE_G,
        ~FILE_F & ~FILE_G & ~FILE_H,
        ~FILE_G & ~FILE_H
    };

    constexpr int32_t BISHOP_PAIR[PHASES] {45, 55};
    constexpr int32_t ROOK_OPEN_FILE[PHASES] {20, 0};
    constexpr int32_t DOUBLED_ROOKS[PHASES] {15, 20};
    constexpr int32_t WEAK_COLOR[PHASES] {-15, 0};
    constexpr int32_t LOST_CASTLING[PHASES] {-40, 0};

    constexpr int32_t ISOLATED_PAWNS[PHASES] {-15, -30};
    constexpr int32_t CONNECTED_PAWNS[PHASES] {12, 20};
    constexpr int32_t DOUBLED_PAWNS[PHASES] {-20, -30};
    constexpr int32_t PASSED_PAWNS[PHASES] {10, 40};
    constexpr int32_t KING_PAWNS_SHIELD[PHASES] {40, 0};

    extern bitboard _pawns_shield_mask[COLOR_SIZE][64];

    bool _has_bishop_pair(const Board &board, Color color); // +45 op, +55 end
    int32_t _rook_on_open_file(const Board &board, Color color); // +20 on opening, skip at endgame
    int32_t _doubled_rooks(const Board &board, Color color); // +15
    int32_t _color_weakness(const Board &board, Color color); // If fields of one color are worse controlled than for another color
    int32_t _lost_castling(const Board &board, Color color); // -50

    // Pawn evaluates
    int32_t _isolated_pawns(const Board &board, Color color); // -10
    int32_t _connected_pawns(const Board &board, Color color); // Pawn protected by another pawn +12
    int32_t _doubled_pawns(const Board &board, Color color); // -25
    int32_t _passed_pawns(const Board &board, Color color);
    int32_t _king_pawns_shield(const Board &board, Color color);

    // I do not know how to code pawn_islands evaluation now

    int32_t _get_pawn_eval(const Board &board, Color color);

    template<GamePhase phase>
    int32_t _phase_evaluation(const Board &board, Color color);

} // hidden

    void init();
    int32_t evaluate(const Board &board, Color color);
}

#endif //ATENICA_EVALUATION_HPP
