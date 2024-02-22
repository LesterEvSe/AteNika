#ifndef ATENIKA_EVAL_HPP
#define ATENIKA_EVAL_HPP

#include "board.hpp"

// MATERIAL_BONUS and PST take from https://www.chessprogramming.org/Simplified_Evaluation_Function
// Other value from https://github.com/bluefeversoft/vice/blob/main/Vice11/src/evaluate.c
namespace Eval {
namespace hidden {

    // Without King
    constexpr int32_t PST[PIECE_SIZE-1][64] = {
            // Pawn
            {
                    0,  0,  0,  0,  0,  0,  0,  0,
                    50, 50, 50, 50, 50, 50, 50, 50,
                    10, 10, 20, 30, 30, 20, 10, 10,
                    5,  5, 10, 25, 25, 10,  5,  5,
                    0,  0,  0, 20, 20,  0,  0,  0,
                    5, -5,-10,  0,  0,-10, -5,  5,
                    5, 10, 10,-20,-20, 10, 10,  5,
                    0,  0,  0,  0,  0,  0,  0,  0
            },

            // Knight
            {
                    -50,-40,-30,-30,-30,-30,-40,-50,
                    -40,-20,  0,  0,  0,  0,-20,-40,
                    -30,  0, 10, 15, 15, 10,  0,-30,
                    -30,  5, 15, 20, 20, 15,  5,-30,
                    -30,  0, 15, 20, 20, 15,  0,-30,
                    -30,  5, 10, 15, 15, 10,  5,-30,
                    -40,-20,  0,  5,  5,  0,-20,-40,
                    -50,-40,-30,-30,-30,-30,-40,-50
            },

            // Bishop
            {
                    -20,-10,-10,-10,-10,-10,-10,-20,
                    -10,  0,  0,  0,  0,  0,  0,-10,
                    -10,  0,  5, 10, 10,  5,  0,-10,
                    -10,  5,  5, 10, 10,  5,  5,-10,
                    -10,  0, 10, 10, 10, 10,  0,-10,
                    -10, 10, 10, 10, 10, 10, 10,-10,
                    -10,  5,  0,  0,  0,  0,  5,-10,
                    -20,-10,-10,-10,-10,-10,-10,-20
            },

            // Rook
            {
                    0,  0,  0,  0,  0,  0,  0,  0,
                    5, 10, 10, 10, 10, 10, 10,  5,
                    -5,  0,  0,  0,  0,  0,  0, -5,
                    -5,  0,  0,  0,  0,  0,  0, -5,
                    -5,  0,  0,  0,  0,  0,  0, -5,
                    -5,  0,  0,  0,  0,  0,  0, -5,
                    -5,  0,  0,  0,  0,  0,  0, -5,
                    0,  0,  0,  5,  5,  0,  0,  0
            },

            // Queen
            {
                    -20,-10,-10, -5, -5,-10,-10,-20,
                    -10,  0,  0,  0,  0,  0,  0,-10,
                    -10,  0,  5,  5,  5,  5,  0,-10,
                    -5,  0,  5,  5,  5,  5,  0, -5,
                    0,  0,  5,  5,  5,  5,  0, -5,
                    -10,  5,  5,  5,  5,  5,  0,-10,
                    -10,  0,  5,  0,  0,  0,  0,-10,
                    -20,-10,-10, -5, -5,-10,-10,-20
            },
    };

    // PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING (last value don't need)
    constexpr int32_t MATERIAL_BONUS[PIECE_SIZE] {
            100, 320, 330, 500, 900, 0
    };

    constexpr int32_t ENDGAME_MAT = MATERIAL_BONUS[ROOK] + 2*MATERIAL_BONUS[KNIGHT] + 2*MATERIAL_BONUS[PAWN];

    // King middle game
    constexpr int32_t KING_M[64] = {
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -20,-30,-30,-40,-40,-30,-30,-20,
            -10,-20,-20,-20,-20,-20,-20,-10,
            20, 20,  0,  0,  0,  0, 20, 20,
            20, 30, 10,  0,  0, 10, 30, 20
    };

    // King endgame
    constexpr int32_t KING_E[64] = {
            -50,-40,-30,-20,-20,-30,-40,-50,
            -30,-20,-10,  0,  0,-10,-20,-30,
            -30,-10, 20, 30, 30, 20,-10,-30,
            -30,-10, 30, 40, 40, 30,-10,-30,
            -30,-10, 30, 40, 40, 30,-10,-30,
            -30,-10, 20, 30, 30, 20,-10,-30,
            -30,-30,  0,  0,  0,  0,-30,-30,
            -50,-30,-30,-30,-30,-30,-30,-50
    };

    constexpr uint8_t FLIP[64] {
            56,  57,  58,  59,  60,  61,  62,  63,
            48,  49,  50,  51,  52,  53,  54,  55,
            40,  41,  42,  43,  44,  45,  46,  47,
            32,  33,  34,  35,  36,  37,  38,  39,
            24,  25,  26,  27,  28,  29,  30,  31,
            16,  17,  18,  19,  20,  21,  22,  23,
            8,   9,  10,  11,  12,  13,  14,  15,
            0,   1,   2,   3,   4,   5,   6,   7
    };

    constexpr int32_t ISOLATED_PAWN = -10;
    constexpr int32_t PASSED_PAWNS[8] = {0, 5, 10, 20, 35, 60, 100, 200}; // Ranks

    constexpr int32_t ROOK_OPEN_FILE = 10;
    constexpr int32_t ROOK_SEMI_OPEN_FILE = 5;

    constexpr int32_t QUEEN_OPEN_FILE = 5;
    constexpr int32_t QUEEN_SEMI_OPEN_FILE = 3;

    constexpr int32_t BISHOP_PAIR = 30;

    // files
    constexpr bitboard ISOLATED_PAWNS_MASK[8] {
        FILE_A | FILE_B,
        FILE_A | FILE_B | FILE_C,
        FILE_B | FILE_C | FILE_D,
        FILE_C | FILE_D | FILE_E,
        FILE_D | FILE_E | FILE_F,
        FILE_E | FILE_F | FILE_G,
        FILE_F | FILE_G | FILE_H,
        FILE_G | FILE_H
    };

    constexpr bitboard COL[] = {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H};

    extern bitboard _wp_passed_mask[64];
    extern bitboard _bp_passed_mask[64];

} // hidden

    void init();
    int32_t evaluate(const Board &board);
} // Eval

#endif //ATENIKA_EVAL_HPP
