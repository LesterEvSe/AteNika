#ifndef ATENIKA_EVAL_HPP
#define ATENIKA_EVAL_HPP

#include "board.hpp"

namespace Eval {
namespace hidden {
    constexpr bitboard ROW[] = {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8};
    constexpr bitboard COL[] = {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H};

    // For Tapered Eval https://www.chessprogramming.org/Tapered_Eval
    // PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
    constexpr uint16_t PHASE_WEIGHTS[6] = { 0, 1, 1, 2, 4, 0 };

    constexpr int32_t MAX_PHASE = 256;
    constexpr int32_t TOTAL_PHASE =
            4*PHASE_WEIGHTS[KNIGHT] + 4*PHASE_WEIGHTS[BISHOP] + 4*PHASE_WEIGHTS[ROOK] + 2*PHASE_WEIGHTS[QUEEN];
    constexpr int32_t HALF_TOTAL_PHASE = TOTAL_PHASE / 2;


    constexpr bitboard FILES_AND_NEIGHBORS[8] = {
        FILE_A | FILE_B,
        FILE_A | FILE_B | FILE_C,
        FILE_B | FILE_C | FILE_D,
        FILE_C | FILE_D | FILE_E,
        FILE_D | FILE_E | FILE_F,
        FILE_E | FILE_F | FILE_G,
        FILE_F | FILE_G | FILE_H,
        FILE_G | FILE_H
    };

    // OPENING and ENDGAME
    // PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
    constexpr int32_t MATERIAL_BONUS[PHASES][PIECE_SIZE] = {
        // Alpha Zero evaluation for classical position in OPENING
        {100, 305, 333, 563, 950, 0},
        {120, 300, 310, 500, 960, 0}
    };

    constexpr int16_t MOBILITY_BONUS[PHASES][PIECE_SIZE] = {
        {0, 0, 4, 3, 1, 0},
        {1, 1, 5, 2, 1, 1}
    };

    // constexpr int32_t WEAK_COLOR[PHASES] {-15, 0};
    // constexpr int32_t LOST_CASTLING[PHASES] {-40, 0};

    // bool _color_weakness(const Board &board, Color color); // Hard to implement
    // int32_t _lost_castling(const Board &board, Color color); // Maybe later

    constexpr int32_t BISHOP_PAIR[PHASES] {45, 55};
    constexpr int32_t DOUBLED_ROOKS[PHASES] {15, 20};
    constexpr int32_t ROOK_OPEN_FILE[PHASES] {10, 0};

    constexpr int32_t ISOLATED_PAWNS[PHASES] {-15, -30};
    constexpr int32_t CONNECTED_PAWNS[PHASES] {12, 20};
    constexpr int32_t DOUBLED_PAWNS[PHASES] {-20, -30};
    constexpr int32_t PASSED_PAWNS[PHASES] {10, 40};
    constexpr int32_t KING_PAWNS_SHIELD[PHASES] {30, 0};

    extern bitboard _pawns_shield_mask[COLOR_SIZE][64];
    extern bitboard _passed_pawns_mask[COLOR_SIZE][64];

    bool _has_bishop_pair(const Board &board, Color color);
    bool _doubled_rooks(const Board &board, Color color);
    uint8_t _rook_on_open_file(const Board &board, Color color);

    // Pawn evaluates, return value - quantity of pawns
    uint8_t _isolated_pawns(const Board &board, Color color);
    uint8_t _connected_pawns(const Board &board, Color color);
    uint8_t _doubled_pawns(const Board &board, Color color);
    uint8_t _passed_pawns(const Board &board, Color color);
    bool _king_pawns_shield(const Board &board, Color color);
    int32_t _get_pawn_eval(const Board &board, Color color, GamePhase phase); // used all pawns evaluation

    int32_t _evaluate_material(const Board &board, Color color, GamePhase phase);
    int32_t _evaluate_mobility(const Board &board, Color color, GamePhase phase);

    int32_t _phase_evaluation(const Board &board, Color color, GamePhase phase);
    int32_t _calculate_phase(const Board &board);

} // hidden

    void init();
    int32_t get_material(PieceType type);
    int32_t evaluate(const Board &board, Color color);
} // Eval

#endif //ATENIKA_EVAL_HPP
