#ifndef CHESSAI_PAWN_HPP
#define CHESSAI_PAWN_HPP

#include "defs.hpp"

namespace Pawn {
    bitboard get_moves(bitboard pawns, bitboard blockers, Color color);

    bitboard get_left_attacks(bitboard pawns, Color color);
    bitboard get_right_attacks(bitboard pawns, Color color);
}

#endif //CHESSAI_PAWN_HPP
