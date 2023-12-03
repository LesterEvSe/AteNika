#include "mvv_lva.hpp"

uint8_t MvvLva::mvv_lva[PIECE_SIZE - 1][PIECE_SIZE];

void MvvLva::init() {
    PieceType MVV[PIECE_SIZE - 1] = {PAWN, KNIGHT, BISHOP, ROOK, QUEEN };
    PieceType LVA[PIECE_SIZE] = {KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };

    uint8_t value = 0;
    for (PieceType victim : MVV)
        for (PieceType attacker : LVA)
            mvv_lva[victim][attacker] = ++value;
}