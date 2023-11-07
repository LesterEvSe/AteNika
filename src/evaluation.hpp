#ifndef ATENICA_EVALUATION_HPP
#define ATENICA_EVALUATION_HPP

#include "board.hpp"

namespace Evaluation {
namespace hidden {
    constexpr bitboard ROW[] = {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8};
    constexpr bitboard COL[] = {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H};

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
    // constexpr int32_t WEAK_COLOR[PHASES] {-15, 0}; // hard to implement
    constexpr int32_t LOST_CASTLING[PHASES] {-40, 0};

    constexpr int32_t ISOLATED_PAWNS[PHASES] {-15, -30};
    constexpr int32_t CONNECTED_PAWNS[PHASES] {12, 20};
    constexpr int32_t DOUBLED_PAWNS[PHASES] {-20, -30};
    constexpr int32_t PASSED_PAWNS[PHASES] {10, 40};
    constexpr int32_t KING_PAWNS_SHIELD[PHASES] {40, 0};

    extern bitboard _pawns_shield_mask[COLOR_SIZE][64];

    bool _has_bishop_pair(const Board &board, Color color);
    uint8_t _rook_on_open_file(const Board &board, Color color);
    bool _doubled_rooks(const Board &board, Color color);
    // bool _color_weakness(const Board &board, Color color); // hard to implement
    int32_t _lost_castling(const Board &board, Color color);

    // Pawn evaluates, quantity of pawns
    uint8_t _isolated_pawns(const Board &board, Color color);
    uint8_t _connected_pawns(const Board &board, Color color);
    uint8_t _doubled_pawns(const Board &board, Color color);
    uint8_t _passed_pawns(const Board &board, Color color);
    bool _king_pawns_shield(const Board &board, Color color);

    int32_t _get_pawn_eval(const Board &board, Color color);

    template<GamePhase phase>
    int32_t _phase_evaluation(const Board &board, Color color);

} // hidden

    void init();
    int32_t evaluate(const Board &board, Color color);

    int32_t get_material_eval(PieceType piece);
}

#endif //ATENICA_EVALUATION_HPP
