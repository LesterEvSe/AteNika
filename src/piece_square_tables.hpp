#ifndef ATENICA_PIECE_SQUARE_TABLES_HPP
#define ATENICA_PIECE_SQUARE_TABLES_HPP

#include "defs.hpp"

// Mix 3 tables in first version
// 1. https://www.chessprogramming.org/Simplified_Evaluation_Function
// 2. https://github.com/GunshipPenguin/shallow-blue
// 3. https://www.chessprogramming.org/PeSTO's_Evaluation_Function
class PieceTables {
private:
    static int32_t piece_sq_tables[COLOR_SIZE][PHASES][PIECE_SIZE][64];

    // PT - Piece Table
    static int32_t *create_pawns_pt   (GamePhase phase);
    static int32_t *create_knights_pt (GamePhase phase);
    static int32_t *create_bishops_pt (GamePhase phase);
    static int32_t *create_rooks_pt   (GamePhase phase);
    static int32_t *create_queens_pt  (GamePhase phase);
    static int32_t *create_king_pt    (GamePhase phase);

    int32_t evals[COLOR_SIZE][PHASES] = {{0}};

public:
    void add_piece(Color color, PieceType piece, uint8_t square);
    void remove_piece(Color color, PieceType piece, uint8_t square);

    static void init();
    int32_t get_eval(Color color, GamePhase phase);
};

#endif //ATENICA_PIECE_SQUARE_TABLES_HPP
