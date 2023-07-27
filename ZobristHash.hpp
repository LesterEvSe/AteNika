#ifndef CHESSAI_ZOBRISTHASH_HPP
#define CHESSAI_ZOBRISTHASH_HPP

#include "global.hpp"
#include <random>

class ZobristHash {
private:
    static constexpr uint64_t seed {1234};
    static std::mt19937_64 gen;
    static std::uniform_int_distribution<uint64_t> dist;

    bitboard constants[64][2][6] {0};
    uint64_t black_move {0};
    uint64_t white_short_castling {0}, white_long_castling {0};
    uint64_t black_short_castling {0}, black_long_castling {0};

    ZobristHash();
public:
    static ZobristHash& get_instance() {
        static ZobristHash instance = ZobristHash();
        return instance;
    }

    // The results must not be ignored
    [[nodiscard]] bitboard operator()(uint8_t i, uint8_t j, uint8_t k) const;
    [[nodiscard]] uint64_t get_black_move() const;
    [[nodiscard]] uint64_t get_white_short_castling() const;
    [[nodiscard]] uint64_t get_white_long_castling() const;
    [[nodiscard]] uint64_t get_black_short_castling() const;
    [[nodiscard]] uint64_t get_black_long_castling() const;
};

#endif //CHESSAI_ZOBRISTHASH_HPP
