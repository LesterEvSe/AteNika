#include "pst.hpp"
#include "board.hpp"
#include <cstring> // for std::memset

int32_t PieceTables::piece_sq_tables[COLOR_SIZE][PIECE_SIZE][64];

void PieceTables::init() {
    // White side representation
    int32_t pawns[PHASES][64] = {
            {  0,  0,  0,  0,  0,  0,  0,  0,
               50, 50, 50, 50, 50, 50, 50, 50,
               10, 10, 20, 30, 30, 20, 10, 10,
               5,  5, 10, 25, 25, 10,  5,  5,
               0,  0,  0, 20, 20,  0,  0,  0,
               5, -5,-10,  0,  0,-10, -5,  5,
               5, 10, 10,-20,-20, 10, 10,  5,
               0,  0,  0,  0,  0,  0,  0,  0}
    };

    int32_t knight[PHASES][64] = {
            {-50,-40,-30,-30,-30,-30,-40,-50,
             -40,-20,  0,  0,  0,  0,-20,-40,
             -30,  0, 10, 15, 15, 10,  0,-30,
             -30,  5, 15, 20, 20, 15,  5,-30,
             -30,  0, 15, 20, 20, 15,  0,-30,
             -30,  5, 10, 15, 15, 10,  5,-30,
             -40,-20,  0,  5,  5,  0,-20,-40,
             -50,-40,-30,-30,-30,-30,-40,-50}
    };

    int32_t bishop[PHASES][64] = {
            {-20,-10,-10,-10,-10,-10,-10,-20,
             -10,  0,  0,  0,  0,  0,  0,-10,
             -10,  0,  5, 10, 10,  5,  0,-10,
             -10,  5,  5, 10, 10,  5,  5,-10,
             -10,  0, 10, 10, 10, 10,  0,-10,
             -10, 10, 10, 10, 10, 10, 10,-10,
             -10,  5,  0,  0,  0,  0,  5,-10,
             -20,-10,-10,-10,-10,-10,-10,-20,}
    };

    int32_t rook[PHASES][64] = {
            { 0,  0,  0,  0,  0,  0,  0,  0,
              5, 10, 10, 10, 10, 10, 10,  5,
              -5,  0,  0,  0,  0,  0,  0, -5,
              -5,  0,  0,  0,  0,  0,  0, -5,
              -5,  0,  0,  0,  0,  0,  0, -5,
              -5,  0,  0,  0,  0,  0,  0, -5,
              -5,  0,  0,  0,  0,  0,  0, -5,
              0,  0,  0,  5,  5,  0,  0,  0}
    };

    int32_t queen[PHASES][64] = {
            {-20,-10,-10, -5, -5,-10,-10,-20,
             -10,  0,  0,  0,  0,  0,  0,-10,
             -10,  0,  5,  5,  5,  5,  0,-10,
             -5,  0,  5,  5,  5,  5,  0, -5,
             0,  0,  5,  5,  5,  5,  0, -5,
             -10,  5,  5,  5,  5,  5,  0,-10,
             -10,  0,  5,  0,  0,  0,  0,-10,
             -20,-10,-10, -5, -5,-10,-10,-20}
    };

    int32_t king[PHASES][64] = {
            {-30,-40,-40,-50,-50,-40,-40,-30,
                    -30,-40,-40,-50,-50,-40,-40,-30,
                    -30,-40,-40,-50,-50,-40,-40,-30,
                    -30,-40,-40,-50,-50,-40,-40,-30,
                    -20,-30,-30,-40,-40,-30,-30,-20,
                    -10,-20,-20,-20,-20,-20,-20,-10,
                    20, 20,  0,  0,  0,  0, 20, 20,
                    20, 30, 10,  0,  0, 10, 30, 20},

            {-50,-40,-30,-20,-20,-30,-40,-50,
                    -30,-20,-10,  0,  0,-10,-20,-30,
                    -30,-10, 20, 30, 30, 20,-10,-30,
                    -30,-10, 30, 40, 40, 30,-10,-30,
                    -30,-10, 30, 40, 40, 30,-10,-30,
                    -30,-10, 20, 30, 30, 20,-10,-30,
                    -30,-30,  0,  0,  0,  0,-30,-30,
                    -50,-30,-30,-30,-30,-30,-30,-50}
    };

    uint8_t FLIP[64] {
            56,  57,  58,  59,  60,  61,  62,  63,
            48,  49,  50,  51,  52,  53,  54,  55,
            40,  41,  42,  43,  44,  45,  46,  47,
            32,  33,  34,  35,  36,  37,  38,  39,
            24,  25,  26,  27,  28,  29,  30,  31,
            16,  17,  18,  19,  20,  21,  22,  23,
            8,   9,  10,  11,  12,  13,  14,  15,
            0,   1,   2,   3,   4,   5,   6,   7
    };

    for (Color color : {WHITE, BLACK})
        for (uint8_t i = 0; i < 64; ++i) {
            uint8_t j = color == WHITE ? FLIP[i] : i;
            piece_sq_tables[color][PAWN][j]   = pawns[OPENING][i];
            piece_sq_tables[color][KNIGHT][j] = knight[OPENING][i];
            piece_sq_tables[color][BISHOP][j] = bishop[OPENING][i];
            piece_sq_tables[color][ROOK][j]   = rook[OPENING][i];
            piece_sq_tables[color][QUEEN][j]  = queen[OPENING][i];
            piece_sq_tables[color][KING][j]   = king[OPENING][i];
        }
}

int32_t PieceTables::get_score(const Board &board)
{
    int32_t score = 0;
    bitboard white = board.get_side_pieces(WHITE);
    bitboard black = board.get_side_pieces(BLACK);

    for (uint8_t i = 0; i < 64; ++i) {
        bitboard bit = ONE << i;

        if (white & bit)
            score += piece_sq_tables[WHITE][board.get_piece_at(WHITE, i)][i];
        else if (black & bit)
            score -= piece_sq_tables[BLACK][board.get_piece_at(BLACK, i)][i];
    }
    return score;
}

/*
void PieceTables::set_scores(const Board &board)
{
    std::memset(m_scores, 0, sizeof(m_scores));
    bitboard white = board.get_side_pieces(WHITE);
    bitboard black = board.get_side_pieces(BLACK);

    for (uint8_t i = 0; i < 64; ++i) {
        bitboard bit = ONE << i;

        if (white & bit)
            add(WHITE, board.get_piece_at(WHITE, i), i);
        else if (black & bit)
            add(BLACK, board.get_piece_at(BLACK, i), i);
    }
}

void PieceTables::add_and_inc(Color color, PieceType piece, uint8_t sq) {
    m_scores[color][OPENING] += piece_sq_tables[color][OPENING][piece][sq];
}
void PieceTables::remove(Color color, PieceType piece, uint8_t sq) {
    m_scores[color][OPENING] -= piece_sq_tables[color][OPENING][piece][sq];
}
int32_t PieceTables::get(Color color, GamePhase phase) const {
    return m_scores[color][phase];
}
*/