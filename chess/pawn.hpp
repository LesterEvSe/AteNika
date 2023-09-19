#ifndef CHESSAI_PAWN_HPP
#define CHESSAI_PAWN_HPP

#include "defs.hpp"

namespace Pawn {
    extern bitboard _pawn_attacks[2][64];

    void init_pawn_attacks();
    bitboard get_moves(bitboard pawns, bitboard blockers, Color color);

    bitboard get_attacks(uint8_t cell, Color color);
    bitboard get_left_attacks(bitboard pawns, Color color);
    bitboard get_right_attacks(bitboard pawns, Color color);
}

#endif //CHESSAI_PAWN_HPP
