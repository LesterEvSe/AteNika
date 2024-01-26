#ifndef ATENIKA_PST_HPP
#define ATENIKA_PST_HPP

#include "defs.hpp"

class Board;

// PST - Piece Square Tables
// Take evaluation from PeSTO's Function https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function
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

    int32_t scores[COLOR_SIZE][PHASES];

public:
    static void init();
    PieceTables() : scores {{0}} {}
    void set_score(const Board &board);

    void add_piece(Color color, PieceType piece, uint8_t square);
    void remove_piece(Color color, PieceType piece, uint8_t square);
    int32_t get_eval(Color color, GamePhase phase);
};

#endif //ATENIKA_PST_HPP
