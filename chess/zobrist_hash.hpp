#ifndef CHESSAI_ZOBRIST_HASH_HPP
#define CHESSAI_ZOBRIST_HASH_HPP

#include "defs.hpp"
#include "board.hpp"
#include <random>

class ZobristHash {
private:
    // Pseudo-Random Number Generator
    static constexpr uint64_t PRNG {1234};
    static std::mt19937_64 generator;
    static std::uniform_int_distribution<uint64_t> distribution;

    // color, piece, cell
    static uint64_t PIECE_KEYS[2][6][64];
    static uint64_t EN_PASSANT_FILE[8];
    static uint64_t QS_CASTLE[2];
    static uint64_t KS_CASTLE[2];
    static uint64_t WHITE_MOVE;

    uint64_t m_hash {ZERO};

public:
    void init();
    ZobristHash(const Board& board);

    [[nodiscard]] uint64_t get_hash() const;
    friend bool operator== (const ZobristHash& left, const ZobristHash& right);

    void xor_piece(Color col, PieceType piece, uint8_t cell);
    void xor_en_passant(uint8_t file);

    void xor_qs_castle(Color color);
    void xor_ks_castle(Color color);

    void xor_move();
};

#endif //CHESSAI_ZOBRIST_HASH_HPP
