#ifndef CHESSAI_ATTACKS_HPP
#define CHESSAI_ATTACKS_HPP

#include "bitboard.hpp"
#include "pieces.hpp"

class Attacks {
private:
    static bitboard KingAttacks[64];
    static bitboard KnightAttacks[64];

    static void calculate_king_attacks();
    static void calculate_knight_attacks();

public:
    static void init();
    static bitboard get_king_moves(const Pieces& pieces, uint8_t cell, bool side);
    static bitboard get_king_captures(const Pieces& pieces, uint8_t cell, bool side);

    static bitboard get_knight_moves(const Pieces& pieces, uint8_t cell, bool side);
    static bitboard get_knight_captures(const Pieces& pieces, uint8_t cell, bool side);

    static bitboard get_white_pawn_default_moves(const Pieces& pieces);
    static bitboard get_black_pawn_default_moves(const Pieces& pieces);

    static bitboard get_white_pawn_long_moves(const Pieces& pieces);
    static bitboard get_black_pawn_long_moves(const Pieces& pieces);

    // all_captures add a check for taking empty cell.
    // May be useful for enemy castling,
    // you can't castle if the castling field are under attack
    static bitboard get_white_pawn_left_captures(const Pieces& pieces, bool all_captures);
    static bitboard get_black_pawn_left_captures(const Pieces& pieces, bool all_captures);

    static bitboard get_white_pawn_right_captures(const Pieces& pieces, bool all_captures);
    static bitboard get_black_pawn_right_captures(const Pieces& pieces, bool all_captures);
};


#endif //CHESSAI_ATTACKS_HPP
