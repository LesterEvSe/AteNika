#ifndef ATENICA_PIECE_SQUARE_TABLES_HPP
#define ATENICA_PIECE_SQUARE_TABLES_HPP

#include "defs.hpp"

// Mix 3 tables in first version
// 1. https://www.chessprogramming.org/Simplified_Evaluation_Function
// 2. https://github.com/GunshipPenguin/shallow-blue
// 3. https://www.chessprogramming.org/PeSTO's_Evaluation_Function
namespace PieceTables {
namespace hidden {
    extern int32_t _piece_sq_tables[COLOR_SIZE][PHASES][PIECE_SIZE][64];

    // PT - Piece Table
    int32_t *_create_pawns_pt   (GamePhase phase);
    int32_t *_create_knights_pt (GamePhase phase);
    int32_t *_create_bishops_pt (GamePhase phase);
    int32_t *_create_rooks_pt   (GamePhase phase);
    int32_t *_create_queens_pt  (GamePhase phase);
    int32_t *_create_king_pt    (GamePhase phase);
} // hidden
    void init();
    int32_t get_eval(Color color, GamePhase phase, PieceType piece, uint8_t square);
}

#endif //ATENICA_PIECE_SQUARE_TABLES_HPP
