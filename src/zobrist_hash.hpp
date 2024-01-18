#ifndef ATENIKA_ZOBRIST_HASH_HPP
#define ATENIKA_ZOBRIST_HASH_HPP

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
    static uint96 PIECE_KEYS[COLOR_SIZE][PIECE_SIZE][64];
    static uint96 EN_PASSANT_FILE[8];
    static uint96 QS_CASTLE[COLOR_SIZE];
    static uint96 KS_CASTLE[COLOR_SIZE];\
    static uint96 WHITE_MOVE;


    // ep - en passant (empty if 1000)
    // 4 bits
    // 0001 - white kingside
    // 0010 - white queenside
    // 0100 - black kingside
    // 1000 - black queenside
    struct {
        uint8_t m_ep_file : 4;
        uint8_t m_castling_rights : 4;
    };
    uint96 m_hash;

public:
    static void init();
    ZobristHash() : m_ep_file(0x8), m_castling_rights(0), m_hash(0, 0) {}
    void set_hash(const Board &board);
    [[nodiscard]] uint96 get_hash() const;
    friend bool operator==(const ZobristHash &left, const ZobristHash &right);

    void xor_piece(Color col, PieceType piece, uint8_t cell);
    void xor_move();

    void xor_en_passant(int8_t file);
    void clear_en_passant();
    void update_castling_rights(uint8_t castling);
};

#endif //ATENIKA_ZOBRIST_HASH_HPP
