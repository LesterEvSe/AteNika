#include "ZobristHash.hpp"

std::mt19937_64 ZobristHash::gen {seed};
std::uniform_int_distribution<uint64_t> ZobristHash::dist;

ZobristHash::ZobristHash() {
    for (auto &matrix : constants)
        for (auto &array : matrix)
            for (auto &val : array)
                val = dist(gen);

    black_move = dist(gen);
    white_short_castling = dist(gen);
    white_long_castling = dist(gen);

    black_short_castling = dist(gen);
    black_long_castling = dist(gen);
}

bitboard ZobristHash::operator()(uint8_t i, uint8_t j, uint8_t k) const {
    if (i > 63 || j > 1 || k > 5) return 0;
    return constants[i][j][k];
}
uint64_t ZobristHash::get_black_move()           const { return black_move;           }
uint64_t ZobristHash::get_white_short_castling() const { return white_short_castling; }
uint64_t ZobristHash::get_white_long_castling()  const { return white_long_castling;  }
uint64_t ZobristHash::get_black_short_castling() const { return black_short_castling; }
uint64_t ZobristHash::get_black_long_castling()  const { return black_long_castling;  }