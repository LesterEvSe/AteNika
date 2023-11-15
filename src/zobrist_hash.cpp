#include "zobrist_hash.hpp"
#include "board.hpp"

std::mt19937_64 ZobristHash::gen64 {PRNG};
std::mt19937 ZobristHash::gen32 {PRNG};
std::uniform_int_distribution<uint64_t> ZobristHash::dist64;
std::uniform_int_distribution<uint32_t> ZobristHash::dist32;

uint96 ZobristHash::PIECE_KEYS[COLOR_SIZE][PIECE_SIZE][64];
uint96 ZobristHash::EN_PASSANT_FILE[8];
uint96 ZobristHash::QS_CASTLE[COLOR_SIZE];
uint96 ZobristHash::KS_CASTLE[COLOR_SIZE];
uint96 ZobristHash::WHITE_MOVE;

void ZobristHash::init() {
    for (uint8_t j = 0; j < PIECE_SIZE; ++j)
        for (uint8_t k = 0; k < 64; ++k) {
            PIECE_KEYS[BLACK][j][k] = {dist64(gen64), dist32(gen32)};
            PIECE_KEYS[WHITE][j][k] = {dist64(gen64), dist32(gen32)};
        }

    for (uint8_t i = 0; i < 8; ++i)
        EN_PASSANT_FILE[i] = {dist64(gen64), dist32(gen32)};

    for (uint8_t i = 0; i < COLOR_SIZE; ++i) {
        QS_CASTLE[i] = {dist64(gen64), dist32(gen32)};
        KS_CASTLE[i] = {dist64(gen64), dist32(gen32)};
    }

    WHITE_MOVE = {dist64(gen64), dist32(gen32)};
}

void ZobristHash::set_hash(const Board &board) {
    if (board.get_curr_move() == WHITE)
        xor_move();
    if (board.get_en_passant())
        xor_en_passant(get_file(board.get_en_passant()));

    if (board.get_white_ks_castle()) {
        m_hash ^= KS_CASTLE[WHITE];
        m_castling_rights |= 1;
    }
    if (board.get_white_qs_castle()) {
        m_hash ^= QS_CASTLE[WHITE];
        m_castling_rights |= 2;
    }
    if (board.get_black_ks_castle()) {
        m_hash ^= KS_CASTLE[BLACK];
        m_castling_rights |= 4;
    }
    if (board.get_black_qs_castle()) {
        m_hash ^= QS_CASTLE[BLACK];
        m_castling_rights |= 8;
    }

    for (uint8_t i = 0; i < PIECE_SIZE; ++i) {
        bitboard white_pieces = board.get_pieces(WHITE, PIECES[i]);
        bitboard black_pieces = board.get_pieces(BLACK, PIECES[i]);

        for (uint8_t j = 0; j < 64; ++j) {
            uint64_t cell = ONE << j;
            if (white_pieces & cell)
                xor_piece(WHITE, PIECES[i], j);
            else if (black_pieces & cell)
                xor_piece(BLACK, PIECES[i], j);
        }
    }
}

uint96 ZobristHash::get_hash() const {
    return m_hash;
}

bool operator==(const ZobristHash &left, const ZobristHash &right) {
    return left.m_hash == right.m_hash;
}

void ZobristHash::xor_piece(Color col, PieceType piece, uint8_t cell) {
    m_hash ^= PIECE_KEYS[col][piece][cell];
}

void ZobristHash::xor_move() {
    m_hash ^= WHITE_MOVE;
}

void ZobristHash::xor_en_passant(int8_t file) {
    m_hash ^= EN_PASSANT_FILE[file];
    m_ep_file = file;
}

void ZobristHash::clear_en_passant() {
    if (m_ep_file != 0x8) {
        m_hash ^= EN_PASSANT_FILE[m_ep_file];
        m_ep_file = 0x8;
    }
}

void ZobristHash::update_castling_rights(uint8_t castling) {
    uint8_t temp = m_castling_rights ^ castling;

    if (temp & 1) {
        m_castling_rights &= 0b1110;
        m_hash ^= KS_CASTLE[WHITE];
    }
    if (temp & 2) {
        m_castling_rights &= 0b1101;
        m_hash ^= QS_CASTLE[WHITE];
    }
    if (temp & 4) {
        m_castling_rights &= 0b1011;
        m_hash ^= KS_CASTLE[BLACK];
    }
    if (temp & 8) {
        m_castling_rights &= 0b0111;
        m_hash ^= QS_CASTLE[BLACK];
    }
}
