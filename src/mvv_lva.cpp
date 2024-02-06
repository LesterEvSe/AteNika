#include "mvv_lva.hpp"

int32_t MvvLva::mvv_lva[PIECE_SIZE-1][PIECE_SIZE];

void MvvLva::init() {
    PieceType victims[] = {PAWN, KNIGHT, BISHOP, ROOK, QUEEN };
    PieceType attackers[] = {KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN};

    // zero, if the king takes a pawn
    int32_t value = 0;
    for (PieceType victim : victims)
        for (PieceType attacker : attackers)
            mvv_lva[victim][attacker] = value++;
}