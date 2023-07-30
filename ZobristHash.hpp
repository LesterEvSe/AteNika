#ifndef CHESSAI_ZOBRISTHASH_HPP
#define CHESSAI_ZOBRISTHASH_HPP

#include "global.hpp"
#include "pieces.hpp"
#include <random>

class Pieces;
class ZobristHash {
private:
    static constexpr uint64_t seed {1234};
    static std::mt19937_64 gen;
    static std::uniform_int_distribution<uint64_t> dist;

    static uint64_t CONSTANTS[64][6][2];
    static uint64_t BLACK_MOVE;
    static uint64_t WHITE_SHORT_CASTLING, WHITE_LONG_CASTLING;
    static uint64_t BLACK_SHORT_CASTLING, BLACK_LONG_CASTLING;

    uint64_t m_hash {0};

public:
    static void init();
    ZobristHash(const Pieces &pieces, bool black_move,
                bool ws_castling, bool wl_castling, bool bs_castling, bool bl_castling);
    friend bool operator==(const ZobristHash& left, const ZobristHash& right);

    // The results must not be ignored
    void xor_piece(uint8_t square, uint8_t side, uint8_t type);
    void xor_move();
    void xor_w_s_castling();
    void xor_w_l_castling();
    void xor_b_s_castling();
    void xor_b_l_castling();
};

#endif //CHESSAI_ZOBRISTHASH_HPP