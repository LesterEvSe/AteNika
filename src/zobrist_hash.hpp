#ifndef ATENICA_ZOBRIST_HASH_HPP
#define ATENICA_ZOBRIST_HASH_HPP

#include "defs.hpp"
#include "uint96.hpp"
#include <random>

class Board;

class ZobristHash {
private:
    // Pseudo-Random Number Generator
    static constexpr uint64_t PRNG {1234};
    static std::mt19937_64 gen64;
    static std::mt19937 gen32;
    static std::uniform_int_distribution<uint64_t> dist64;
    static std::uniform_int_distribution<uint32_t> dist32;

    // color, piece, cell
    static uint96 PIECE_KEYS[2][6][64];
    static uint96 EN_PASSANT_FILE[8];
    static uint96 QS_CASTLE[2];
    static uint96 KS_CASTLE[2];
    static uint96 WHITE_MOVE;

    uint96 m_hash {ZERO};

public:
    static void init();
    ZobristHash() = default;
    void set_hash(const Board &board);
    friend bool operator== (const ZobristHash& left, const ZobristHash& right);

    void xor_piece(Color col, PieceType piece, uint8_t cell);
    void xor_en_passant(uint8_t file);

    void xor_qs_castle(Color color);
    void xor_ks_castle(Color color);

    void xor_move();
};

#endif //ATENICA_ZOBRIST_HASH_HPP
