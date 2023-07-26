#ifndef CHESSAI_PIECES_HPP
#define CHESSAI_PIECES_HPP

#include "global.hpp"
#include <bit> // for std::popcount
#include <string>

class Pieces
{
private:
    // The method for counting is taken from the website: https://www.chessprogramming.org/BitScan
    static constexpr uint8_t BitScan[64] = {
       0, 47,  1, 56, 48, 27,  2, 60,
       57, 49, 41, 37, 28, 16,  3, 61,
       54, 58, 35, 52, 50, 42, 21, 44,
       38, 32, 29, 23, 17, 11,  4, 62,
       46, 55, 26, 59, 40, 36, 15, 53,
       34, 51, 20, 43, 31, 22, 10, 45,
       25, 39, 14, 33, 19, 30,  9, 24,
       13, 18,  8, 12,  7,  6,  5, 63
    };

    // The top 18 bitboards to work

    // all pieces, black and white (black pawns, white bishops ...)
    bitboard s_pieces_bitboards[2][6]{};

    // white and black pieces
    bitboard s_side[2]{};
    bitboard s_inverse_side[2]{};

    // all the pieces on the board
    bitboard s_all{};
    bitboard s_empty{};

    static constexpr uint8_t PAWN = 0;
    static constexpr uint8_t ROOK = 1;
    static constexpr uint8_t KNIGHT = 2;
    static constexpr uint8_t BISHOP = 3;
    static constexpr uint8_t KING = 4;
    static constexpr uint8_t QUEEN = 5;

    static constexpr uint8_t BLACK = 0;
    static constexpr uint8_t WHITE = 1;

public:
    // using short FEN (Forsyth-Edwards Notation):
    // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
    explicit Pieces(const std::string& short_fen);
    void update_bitboards();
    void print() const;

    static constexpr uint8_t inverse(uint8_t side);
    static constexpr void set0(bitboard& field, uint8_t pos);
    static constexpr void set1(bitboard& field, uint8_t pos);
    static constexpr bool get(bitboard field, uint8_t pos);
    static constexpr uint8_t count1(bitboard field);

    static constexpr uint8_t bsf(bitboard bb);
    static constexpr uint8_t bsr(bitboard bb);
};

#endif //CHESSAI_PIECES_HPP
