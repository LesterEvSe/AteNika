#include "piece_square_tables.hpp"

int32_t PieceTables::hidden::_piece_sq_tables[COLOR_SIZE][PHASES][PIECE_SIZE][64];

int32_t *PieceTables::hidden::_create_pawns_pt(GamePhase phase) {
    if (phase == OPENING)
        return new int32_t[64] {
            0,  0,  0,  0,  0,  0,  0,  0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5,  5, 10, 25, 25, 10,  5,  5,
            0,  0,  0, 20, 20,  0,  0,  0,
            5, -5,-10,  0,  0,-10, -5,  5,
            5, 10, 10,-20,-20, 10, 10,  5,
            0,  0,  0,  0,  0,  0,  0,  0
        };
    return new int32_t[64] {
        0,   0,  0,    0,   0,   0,   0,   0,
        138, 133, 118, 94, 107, 92, 125, 147,
        94, 100,  85,  67,  56,  53,  82,  84,
        40,  40,  40,  40,  40,  40,  40,  40,
        20,  20,  20,  20,  20,  20,  20,  20,
        0,   0,   0,   0,   0,   0,   0,   0,
        -20, -20, -20, -20, -20, -20, -20, -20,
        0,   0,   0,   0,   0,   0,   0,   0
    };
}
int32_t *PieceTables::hidden::_create_knights_pt(GamePhase phase) {
    if (phase == OPENING)
        return new int32_t[64] {
            -167,-40,-30,-30,-30,-30,-40,-107,
             -40,-20,  0,  0,  0,  0,-20, -40,
             -30,  0, 10, 15, 15, 10,  0, -30,
             -30,  5, 15, 20, 20, 15,  5, -30,
             -30,  0, 15, 20, 20, 15,  0, -30,
             -30,  5, 10, 15, 15, 10,  5, -30,
             -40,-20,  0,  5,  5,  0,-20, -40,
            -105,-40,-30,-30,-30,-30,-40, -50,
        };
    return new int32_t[64] {
        -58,-38,-13,-28,-31,-27,-63,-99,
        -25, -8,-25, -2, -9,-25,-24,-52,
        -24,-20, 10,  9, -1, -9,-19,-41,
        -17,  3, 22, 22, 22, 11,  8,-18,
        -18, -6, 16, 25, 16, 17,  4,-18,
        -23, -3, -1, 15, 10, -3,-20,-22,
        -42,-20,-10, -5, -2,-20,-23,-44,
        -29,-51,-23,-15,-22,-18,-50,-64,
    };
}
int32_t *PieceTables::hidden::_create_bishops_pt(GamePhase phase) {
    if (phase == OPENING)
        return new int32_t[64] {
            -20,-10,-10,-10,-10,-10,-10,-20,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -10,  0,  5, 10, 10,  5,  0,-10,
            -10,  5,  5, 10, 10,  5,  5,-10,
            -10,  0, 10, 10, 10, 10,  0,-10,
            -10, 10, 10, 10, 10, 10, 10,-10,
            -10,  5,  0,  0,  0,  0,  5,-10,
            -20,-10,-10,-10,-10,-10,-10,-20,
        };
    return new int32_t[64] {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20,
    };
}
int32_t *PieceTables::hidden::_create_rooks_pt(GamePhase phase) {
    if (phase == OPENING)
        return new int32_t[64] {
             0,  0,  0,  0,  0,  0,  0,  0,
             5, 10, 10, 10, 10, 10, 10,  5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
             0,  0,  0,  5,  5,  0,  0,  0
        };
    return new int32_t[64] {
         0, 0, 0, 0, 0, 0, 0,  0,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
         0, 0, 0, 0, 0, 0, 0,  0,
    };
}
int32_t *PieceTables::hidden::_create_queens_pt(GamePhase phase) {
    if (phase == OPENING)
        return new int32_t[64] {
            -20,-10,-10, -5, -5,-10,-10,-20,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -10,  0,  5,  5,  5,  5,  0,-10,
             -5,  0,  5,  5,  5,  5,  0, -5,
              0,  0,  5,  5,  5,  5,  0, -5,
            -10,  5,  5,  5,  5,  5,  0,-10,
            -10,  0,  5,  0,  0,  0,  0,-10,
            -20,-10,-10, -5, -5,-10,-10,-20
        };
    return new int32_t[64] {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
          0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };
}
int32_t *PieceTables::hidden::_create_king_pt(GamePhase phase) {
    if (phase == OPENING)
        return new int32_t[64] {
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -20,-30,-30,-40,-40,-30,-30,-20,
            -10,-20,-20,-20,-20,-20,-20,-10,
             20, 20,  0,  0,  0,  0, 20, 20,
             20, 30, 10,  0,  0, 10, 30, 20
        };
    return new int32_t[64] {
        -50,-40,-30,-20,-20,-30,-40,-50,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-30,  0,  0,  0,  0,-30,-30,
        -50,-30,-30,-30,-30,-30,-30,-50
    };
}

void PieceTables::init() {
    /* Use to calculate WHITE pieces.
    For example PAWN_PT[ENDGAME][10] = 80. For white, it's start position,
    but if it was flip[10] it was square 50. And that's
    the penultimate line to pawn promotion!*/
    const uint8_t flip[64] = {
        56,  57,  58,  59,  60,  61,  62,  63,
        48,  49,  50,  51,  52,  53,  54,  55,
        40,  41,  42,  43,  44,  45,  46,  47,
        32,  33,  34,  35,  36,  37,  38,  39,
        24,  25,  26,  27,  28,  29,  30,  31,
        16,  17,  18,  19,  20,  21,  22,  23,
        8,   9,  10,  11,  12,  13,  14,  15,
        0,   1,   2,   3,   4,   5,   6,   7
    };

    int32_t *(*table[PIECE_SIZE])(GamePhase);
    table[PAWN]   = hidden::_create_pawns_pt;
    table[KNIGHT] = hidden::_create_knights_pt;
    table[BISHOP] = hidden::_create_bishops_pt;
    table[ROOK]   = hidden::_create_rooks_pt;
    table[QUEEN]  = hidden::_create_queens_pt;
    table[KING]   = hidden::_create_king_pt;

    for (uint8_t ph = 0; ph < PHASES; ++ph)
        for (uint8_t pc = 0; pc < PIECE_SIZE; ++pc) {
            int32_t *temp_table = table[pc]((GamePhase)ph);

            for (uint8_t i = 0; i < 64; ++i) {
                hidden::_piece_sq_tables[BLACK][ph][pc][i] = temp_table[i];
                hidden::_piece_sq_tables[WHITE][ph][pc][i] = temp_table[flip[i]];
            }
            delete[] temp_table;
        }
}

int32_t PieceTables::get_eval(Color color, GamePhase phase, PieceType piece, uint8_t square) {
    return hidden::_piece_sq_tables[color][phase][piece][square];
}
