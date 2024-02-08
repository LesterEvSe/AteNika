#include "pst.hpp"
#include "board.hpp"
#include <cstring> // for std::memset

int32_t PieceTables::piece_sq_tables[COLOR_SIZE][PHASES][PIECE_SIZE][64];

void PieceTables::init() {
    // White side representation
    int32_t pawns[PHASES][64] = {
            {0,0,0,0,0,0,0,0,
             3,3,10,19,16,19,7,-5,
             -9,-15,11,15,32,22,5,-22,
             -4,-23,6,20,40,17,4,-8,
             13,0,-13,1,11,-2,-13,5,
             5,-12,-7,22,-8,-5,-15,-8,
             -7,7,-3,-13,5,-16,10,-8,
             0,0,0,0,0,0,0,0},

            {0,0,0,0,0,0,0,0,
             -10,-6,10,0,14,7,-5,-19,
             -10,-10,-10,4,4,3,-6,-4,
             6,-2,-8,-4,-13,-12,-10,-9,
             10,5,4,-5,-5,-5,14,9,
             28,20,21,28,30,7,6,13,
             0,-11,12,21,25,19,4,7,
             0,0,0,0,0,0,0,0}
    };

    int32_t knight[PHASES][64] = {
            {-175,-92,-74,-73,
             -77,-41,-27,-15,
             -61,-17,6,12,
             -35,8,40,49,
             -34,13,44,51,
             -9,22,58,53,
             -67,-27,4,37,
             -201,-83,-56,-26},

            {-96,-65,-49,-21,
             -67,-54,-18,8,
             -40,-27,-8,29,
             -35,-2,13,28,
             -45,-16,9,39,
             -51,-44,-16,17,
             -69,-50,-51,12,
             -100,-88,-56,-17}
    };

    int32_t bishop[PHASES][64] = {
            {-53,-5,-8,-23,
             -15,8,19,4,
             -7,21,-5,17,
             -5,11,25,39,
             -12,29,22,31,
             -16,6,1,11,
             -17,-14,5,0,
             -48,1,-14,-23},

            {-57,-30,-37,-12,
             -37,-13,-17,1,
             -16,-1,-2,10,
             -20,-6,0,17,
             -17,-1,-14,15,
             -30,6,4,6,
             -31,-20,-1,1,
             -46,-42,-37,-24}
    };

    int32_t rook[PHASES][64] = {
            {-31,-20,-14,-5,
             -21,-13,-8,6,
             -25,-11,-1,3,
             -13,-5,-4,-6,
             -27,-15,-4,3,
             -22,-2,6,12,
             -2,12,16,18,
             -17,-19,-1,9},

            {-9,-13,-10,-9,
             -12,-9,-1,-2,
             6,-8,-2,-6,
             -6,1,-9,7,
             -5,8,7,-6,
             6,1,-7,10,
             4,5,20,-5,
             18,0,19,13}
    };

    int32_t queen[PHASES][64] = {
            {3,-5,-5,4,
             -3,5,8,12,
             -3,6,13,7,
             4,5,9,8,
             0,14,12,5,
             -4,10,6,8,
             -5,6,10,8,
             -2,-2,1,-2},

            {-69,-57,-47,-26,
             -55,-31,-22,-4,
             -39,-18,-9,3,
             -23,-3,13,24,
             -29,-6,9,21,
             -38,-18,-12,1,
             -50,-27,-24,-8,
             -75,-52,-43,-36}
    };

    int32_t king[PHASES][64] = {
            {271,327,271,198,
             278,303,234,179,
             195,258,169,120,
             164,190,138,98,
             154,179,105,70,
             123,145,81,31,
             88,120,65,33,
             59,89,45,-1},

            {1,45,85,76,
             53,100,133,135,
             88,130,169,175,
             103,156,172,172,
             96,166,199,199,
             92,172,184,191,
             47,121,116,131,
             11,59,73,78}
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

    for (GamePhase phase : {OPENING, ENDGAME})
        for (uint8_t i = 0; i < 64; ++i) {
            piece_sq_tables[WHITE][phase][PAWN][i] = pawns[phase][i];
            piece_sq_tables[BLACK][phase][PAWN][i] = pawns[phase][FLIP[i]];
        }

    for (Color color : {WHITE, BLACK})
        for (GamePhase phase : {OPENING, ENDGAME})
            for (uint8_t i = 0, k = 0; i < 8; ++i)
                for ( uint8_t j = color == WHITE ? i*8 : FLIP[i*8], t = j+7; j < t; ++j, --t, ++k) {

                    piece_sq_tables[color][phase][KNIGHT][j] = piece_sq_tables[color][phase][KNIGHT][t] = knight[phase][k];
                    piece_sq_tables[color][phase][BISHOP][j] = piece_sq_tables[color][phase][BISHOP][t] = bishop[phase][k];
                    piece_sq_tables[color][phase][ROOK][j] = piece_sq_tables[color][phase][ROOK][t] = rook[phase][k];
                    piece_sq_tables[color][phase][QUEEN][j] = piece_sq_tables[color][phase][QUEEN][t] = queen[phase][k];
                    piece_sq_tables[color][phase][KING][j] = piece_sq_tables[color][phase][KING][t] = king[phase][k];
                }
}

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

void PieceTables::add(Color color, PieceType piece, uint8_t sq) {
    m_scores[color][OPENING] += piece_sq_tables[color][OPENING][piece][sq];
    m_scores[color][ENDGAME] += piece_sq_tables[color][ENDGAME][piece][sq];
}
void PieceTables::remove(Color color, PieceType piece, uint8_t sq) {
    m_scores[color][OPENING] -= piece_sq_tables[color][OPENING][piece][sq];
    m_scores[color][ENDGAME] -= piece_sq_tables[color][ENDGAME][piece][sq];
}
int32_t PieceTables::get(Color color, GamePhase phase) const {
    return m_scores[color][phase];
}