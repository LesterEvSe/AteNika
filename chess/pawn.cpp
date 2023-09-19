#include "pawn.hpp"

bitboard Pawn::get_moves(bitboard pawns, bitboard blockers, Color color) {
    // If after the move pawn stayed on the 3th row for WHITE
    // and 6th row for BLACK
    blockers = ~blockers;
    switch (color)
    {
        case WHITE: {
            bitboard one_step = (pawns << 8) & blockers;
            return one_step | ((one_step & RANK_3) << 8) & blockers;
        }
        default: {
            bitboard one_step = (pawns >> 8) & blockers;
            return one_step | ((one_step & RANK_6) >> 8) & blockers;
        }
    }
}

bitboard Pawn::get_left_attacks(bitboard pawns, Color color) {
    switch (color) {
        case WHITE: return (pawns << 7) & ~FILE_H;
        default:    return (pawns >> 9) & ~FILE_H;
    }
}

bitboard Pawn::get_right_attacks(bitboard pawns, Color color) {
    switch (color) {
        case WHITE: return (pawns << 9) & ~FILE_A;
        default   : return (pawns >> 7) & ~FILE_A;
    }
}