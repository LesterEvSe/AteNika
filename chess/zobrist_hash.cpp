#include "zobrist_hash.hpp"

std::mt19937_64 ZobristHash::generator {PRNG};
std::uniform_int_distribution<uint64_t> ZobristHash::distribution;

uint64_t ZobristHash::PIECE_KEYS[2][6][64];
uint64_t ZobristHash::EN_PASSANT_FILE[8];
uint64_t ZobristHash::QS_CASTLE[2];
uint64_t ZobristHash::KS_CASTLE[2];
uint64_t ZobristHash::WHITE_MOVE;

void ZobristHash::init() {
    for (uint8_t j = 0; j < 6; ++j)
        for (uint8_t k = 0; k < 64; ++k) {
            PIECE_KEYS[BLACK][j][k] = distribution(generator);
            PIECE_KEYS[WHITE][j][k] = distribution(generator);
        }

    for (uint8_t i = 0; i < 8; ++i)
        EN_PASSANT_FILE[i] = distribution(generator);

    for (uint8_t i = 0; i < 2; ++i) {
        QS_CASTLE[i] = distribution(generator);
        KS_CASTLE[i] = distribution(generator);
    }

    WHITE_MOVE = distribution(generator);
}

ZobristHash::ZobristHash(const Board &board) {

}

uint64_t ZobristHash::get_hash() const { return m_hash; }

bool operator==(const ZobristHash &left, const ZobristHash &right) {
    return left.m_hash == right.m_hash;
}


void ZobristHash::xor_piece(Color col, PieceType piece, uint8_t cell) {
    m_hash ^= PIECE_KEYS[col][piece][cell];
}

void ZobristHash::xor_en_passant(uint8_t file) {
    m_hash ^= EN_PASSANT_FILE[file];
}

void ZobristHash::xor_qs_castle(Color color) {
    m_hash ^= QS_CASTLE[color];
}

void ZobristHash::xor_ks_castle(Color color) {
    m_hash ^= KS_CASTLE[color];
}

void ZobristHash::xor_move() {
    m_hash ^= WHITE_MOVE;
}
