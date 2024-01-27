#include "pst.hpp"
#include "board.hpp"
#include <functional> // for std::function

int32_t PieceTables::piece_sq_tables[COLOR_SIZE][PHASES][PIECE_SIZE][64];

int32_t *PieceTables::create_pawns_pt(GamePhase phase) {
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
        0,   0,  0,  0,  0,  0,  0,  0,
        80, 80, 80, 80, 80, 80, 80, 80,
        60, 60, 60, 60, 60, 60, 60, 60,
        40, 40, 40, 40, 40, 40, 40, 40,
        20, 20, 20, 20, 20, 20, 20, 20,
        0,  0,  0,  0,  0,  0,  0,  0,
        -20,-20,-20,-20,-20,-20,-20,-20,
        0,  0,  0,  0,  0,  0,  0,  0
    };
}
int32_t *PieceTables::create_knights_pt(GamePhase phase) {
    if (phase == OPENING)
        return new int32_t[64] {
            -50,-40,-30,-30,-30,-30,-40,-50,
            -40,-20,  0,  0,  0,  0,-20,-40,
            -30,  0, 10, 15, 15, 10,  0,-30,
            -30,  5, 15, 20, 20, 15,  5,-30,
            -30,  0, 15, 20, 20, 15,  0,-30,
            -30,  5, 10, 15, 15, 10,  5,-30,
            -40,-20,  0,  5,  5,  0,-20,-40,
            -50,-40,-30,-30,-30,-30,-40,-50
        };
    return new int32_t[64] {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50,
    };
}
int32_t *PieceTables::create_bishops_pt(GamePhase phase) {
    if (phase == OPENING)
        return new int32_t[64] {
            -20,-10,-10,-10,-10,-10,-10,-20,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -10,  0,  5, 10, 10,  5,  0,-10,
            -10,  5,  5, 10, 10,  5,  5,-10,
            -10,  0, 10, 10, 10, 10,  0,-10,
            -10, 10, 10, 10, 10, 10, 10,-10,
            -10,  5,  0,  0,  0,  0,  5,-10,
            -20,-10,-10,-10,-10,-10,-10,-20
        };
    return new int32_t[64] {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };
}
int32_t *PieceTables::create_rooks_pt(GamePhase phase) {
    if (phase == OPENING)
        return new int32_t[64] {
            0,  0,  0,  0,  0,  0,  0,  0,
            5,  0,  0,  0,  0,  0,  0,  5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            0,  0,  0,  5,  5,  0,  0,  0
        };
    return new int32_t[64] {
        0,  0,  0,  0,  0,  0,  0,  0,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        0,  0,  0,  0,  0,  0,  0,  0
    };

}
int32_t *PieceTables::create_queens_pt(GamePhase phase) {
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
int32_t *PieceTables::create_king_pt(GamePhase phase) {
    if (phase == OPENING)
        return new int32_t[64] {
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -20,-30,-30,-40,-40,-30,-30,-20,
            -10,-20,-20,-20,-20,-20,-20,-10,
            20, 20,  0,  0,  0,  0, 20, 20,
            30, 30, 20,  20, 20, 30, 30, 30
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
    the penultimate line to pawn promotion! */
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

    // int32_t *(*table[PIECE_SIZE])(GamePhase); // Poorly readable
    std::function<int32_t*(GamePhase)> table[PIECE_SIZE];

    table[PAWN]   = create_pawns_pt;
    table[KNIGHT] = create_knights_pt;
    table[BISHOP] = create_bishops_pt;
    table[ROOK]   = create_rooks_pt;
    table[QUEEN]  = create_queens_pt;
    table[KING]   = create_king_pt;

    for (uint8_t ph = 0; ph < PHASES; ++ph)
        for (uint8_t pc = 0; pc < PIECE_SIZE; ++pc) {
            int32_t *temp_table = table[pc](static_cast<GamePhase>(ph));

            for (uint8_t i = 0; i < 64; ++i) {
                piece_sq_tables[BLACK][ph][pc][i] = temp_table[i];
                piece_sq_tables[WHITE][ph][pc][i] = temp_table[flip[i]];
            }
            delete[] temp_table;
        }
}

void PieceTables::set_score(const Board &board) {
    for (uint8_t i = 0; i < PIECE_SIZE; ++i) {
        bitboard white_pieces = board.get_pieces(WHITE, PIECES[i]);
        bitboard black_pieces = board.get_pieces(BLACK, PIECES[i]);

        for (uint8_t j = 0; j < 64; ++j) {
            uint64_t square = ONE << j;
            if (white_pieces & square)
                add_piece(WHITE, PIECES[i], j);
            else if (black_pieces & square)
                add_piece(BLACK, PIECES[i], j);
        }
    }
}

void PieceTables::add_piece(Color color, PieceType piece, uint8_t square) {
    scores[color][OPENING] += piece_sq_tables[color][OPENING][piece][square];
    scores[color][ENDGAME] += piece_sq_tables[color][ENDGAME][piece][square];
}
void PieceTables::remove_piece(Color color, PieceType piece, uint8_t square) {
    scores[color][OPENING] -= piece_sq_tables[color][OPENING][piece][square];
    scores[color][ENDGAME] -= piece_sq_tables[color][ENDGAME][piece][square];
}

int32_t PieceTables::get_eval(Color color, GamePhase phase) {
    return scores[color][phase];
}
