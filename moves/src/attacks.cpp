#include "attacks.hpp"

bitboard Attacks::KingAttacks[64];
bitboard Attacks::KnightAttacks[64];

void Attacks::calculate_king_attacks() {
    for (uint8_t i = 0; i < 64; ++i) {
        bitboard bit = ONE << i;
        KingAttacks[i] = (bit << 7 | bit >> 1 | bit >> 9) & ~COL_H |
                         (bit << 8 | bit >> 8) |
                         (bit << 9 | bit << 1 | bit >> 7) & ~COL_A;
    }
}

void Attacks::calculate_knight_attacks() {
    for (int i = 0; i < 64; ++i) {
        bitboard bit = ONE << i;

        // lr - left and right shift
        bitboard lr1 = (bit >> 1) & ~COL_H | // left
                       (bit << 1) & ~COL_A;  // right
        bitboard lr2 = (bit >> 2) & ~(COL_G | COL_H) |
                       (bit << 2) & ~(COL_A | COL_B);
        KnightAttacks[i] = lr1 << 16 | lr1 >> 16 | lr2 << 8 | lr2 >> 8;
    }
}



void Attacks::init() {
    calculate_king_attacks();
    calculate_knight_attacks();
}

bitboard Attacks::get_king_attacks(uint8_t cell) {
    return KingAttacks[cell];
}

bitboard Attacks::get_knight_attacks(uint8_t cell) {
    return KnightAttacks[cell];
}
