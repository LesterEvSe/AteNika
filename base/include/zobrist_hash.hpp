#ifndef CHESSAI_ZOBRIST_HASH_HPP
#define CHESSAI_ZOBRIST_HASH_HPP

#include "bitboard.hpp"
#include "pieces.hpp"
#include <random>

class Pieces;
class ZobristHash {
private:
    static constexpr uint64_t seed {1234};
    static std::mt19937_64 generator;
    static std::uniform_int_distribution<uint64_t> distribution;

    static bool called_init;
    static uint64_t CONSTANTS[64][6][2];
    static uint64_t BLACK_MOVE;
    static uint64_t WHITE_SHORT_CASTLING, WHITE_LONG_CASTLING;
    static uint64_t BLACK_SHORT_CASTLING, BLACK_LONG_CASTLING;

    uint64_t m_hash {0};

public:
    static void init();
    ZobristHash(const Pieces &pieces, bool black_move, bool ws_castling,
                bool wl_castling, bool bs_castling, bool bl_castling);

    friend bool operator==(const ZobristHash& left, const ZobristHash& right);
    [[nodiscard]] uint64_t get_hash() const;

    // The results must not be ignored
    void xor_piece(uint8_t square, uint8_t side, uint8_t type);
    void xor_move();
    void xor_ws_castling();
    void xor_wl_castling();
    void xor_bs_castling();
    void xor_bl_castling();
};

#endif //CHESSAI_ZOBRIST_HASH_HPP
