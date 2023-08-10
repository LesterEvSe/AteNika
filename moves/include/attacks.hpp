#ifndef CHESSAI_ATTACKS_HPP
#define CHESSAI_ATTACKS_HPP

#include "bitboard.hpp"

class Attacks {
private:
    static bitboard KingAttacks[64];
    static bitboard KnightAttacks[64];

    static void calculate_king_attacks();
    static void calculate_knight_attacks();

public:
    static void init();
    static bitboard get_king_attacks(uint8_t cell);
    static bitboard get_knight_attacks(uint8_t cell);
};


#endif //CHESSAI_ATTACKS_HPP
