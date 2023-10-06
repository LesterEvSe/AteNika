#ifndef ATENICA_DEFS_HPP
#define ATENICA_DEFS_HPP

#include <cstdint>
#include <string>

/** Arrangements for the presentation of the board (uint64_t) */
/**
 *      A   B   C   D   E   F   G   H
 * 8 | 56  57  58  59  60  61  62  63
 * 7 | 48  49  50  51  52  53  54  55
 * 6 | 40  41  42  43  44  45  46  47
 * 5 | 32  33  34  35  36  37  38  39
 * 4 | 24  25  26  27  28  29  30  31
 * 3 | 16  17  18  19  20  21  22  23
 * 2 |  8   9  10  11  12  13  14  15
 * 1 |  0   1   2   3   4   5   6   7
 */

/** Fen-Notation
 * https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation (without full move counter)
 * rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0  (start position)
       A B C D E F G H
   8 | r n b q k b n r
   7 | p p p p p p p p
   6 | - - - - - - - -
   5 | - - - - - - - -
   4 | - - - - - - - -
   3 | - - - - - - - -
   2 | P P P P P P P P
   1 | R N B Q K B N R
*/

constexpr std::string FIELD[64] = {
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
        "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
        "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
        "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
        "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
        "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
        "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
        "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
};

using bitboard = uint64_t;

constexpr bitboard FILE_A = 0x0101010101010101ULL;
constexpr bitboard FILE_B = FILE_A << 1;
constexpr bitboard FILE_C = FILE_A << 2;
constexpr bitboard FILE_D = FILE_A << 3;
constexpr bitboard FILE_E = FILE_A << 4;
constexpr bitboard FILE_F = FILE_A << 5;
constexpr bitboard FILE_G = FILE_A << 6;
constexpr bitboard FILE_H = FILE_A << 7;

constexpr bitboard RANK_1 = 0xFFULL;
constexpr bitboard RANK_2 = RANK_1 << (8 * 1);
constexpr bitboard RANK_3 = RANK_1 << (8 * 2);
constexpr bitboard RANK_4 = RANK_1 << (8 * 3);
constexpr bitboard RANK_5 = RANK_1 << (8 * 4);
constexpr bitboard RANK_6 = RANK_1 << (8 * 5);
constexpr bitboard RANK_7 = RANK_1 << (8 * 6);
constexpr bitboard RANK_8 = RANK_1 << (8 * 7);

// Order is important!
enum PieceType : uint8_t {
    PAWN,
    BISHOP,
    KNIGHT,
    KING,
    ROOK,
    QUEEN,
    NONE // at the end
};

enum Color : uint8_t {
    BLACK,
    WHITE
};

constexpr uint64_t ZERO {0};
constexpr uint64_t ONE  {1};

#endif //ATENICA_DEFS_HPP
