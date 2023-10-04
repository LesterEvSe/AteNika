#include "zobrist_hash.hpp"
#include "board.hpp"

std::mt19937_64 ZobristHash::gen64 {PRNG};
std::mt19937 ZobristHash::gen32 {PRNG};
std::uniform_int_distribution<uint64_t> ZobristHash::dist64;
std::uniform_int_distribution<uint32_t> ZobristHash::dist32;

uint96 ZobristHash::PIECE_KEYS[2][6][64];
uint96 ZobristHash::EN_PASSANT_FILE[8];
uint96 ZobristHash::QS_CASTLE[2];
uint96 ZobristHash::KS_CASTLE[2];
uint96 ZobristHash::WHITE_MOVE;

void ZobristHash::init() {
    for (uint8_t j = 0; j < 6; ++j)
        for (uint8_t k = 0; k < 64; ++k) {
            PIECE_KEYS[BLACK][j][k] = {dist64(gen64), dist32(gen32)};
            PIECE_KEYS[WHITE][j][k] = {dist64(gen64), dist32(gen32)};
        }

    for (uint8_t i = 0; i < 8; ++i)
        EN_PASSANT_FILE[i] = {dist64(gen64), dist32(gen32)};

    for (uint8_t i = 0; i < 2; ++i) {
        QS_CASTLE[i] = {dist64(gen64), dist32(gen32)};
        KS_CASTLE[i] = {dist64(gen64), dist32(gen32)};
    }

    WHITE_MOVE = {dist64(gen64), dist32(gen32)};
}

// TODO. Do we need it?
ZobristHash::ZobristHash(const Board &board) {
    set_hash(board);
}

void ZobristHash::set_hash(const Board &board) {
    if (board.get_curr_player_move() == WHITE)
        xor_move();
    xor_en_passant(get_col(board.get_en_passant_cell()));

    if (board.get_white_qs_castle())
        xor_qs_castle(WHITE);
    if (board.get_white_ks_castle())
        xor_ks_castle(WHITE);

    if (board.get_black_qs_castle())
        xor_qs_castle(BLACK);
    if (board.get_black_ks_castle())
        xor_ks_castle(BLACK);

    static constexpr PieceType type[6] = { PAWN, ROOK, KNIGHT, BISHOP, KING, QUEEN };
    for (uint8_t i = 0; i < 6; ++i) {
        bitboard white_pieces = board.get_pieces(WHITE, type[i]);
        bitboard black_pieces = board.get_pieces(BLACK, type[i]);

        for (uint8_t j = 0; j < 64; ++j) {
            uint64_t cell = ONE << j;
            if (white_pieces & cell)
                xor_piece(WHITE, type[i], j);
            if (black_pieces & cell)
                xor_piece(BLACK, type[i], j);
        }
    }
}

// TODO. Do we need it?
uint96 ZobristHash::get_hash() const { return m_hash; }

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
