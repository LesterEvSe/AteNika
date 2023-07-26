#include "pieces.hpp"
#include <cctype> // for isdigit function

uint8_t BitScan[64];


constexpr uint8_t Pieces::inverse(uint8_t side) {
    return !side;
}

constexpr void Pieces::set0(bitboard& field, uint8_t pos) {
    field &= ~(bitboard(1) << pos);
}

constexpr void Pieces::set1(bitboard& field, uint8_t pos) {
    field |= bitboard(1) << pos;
}

constexpr bool Pieces::get(bitboard field, uint8_t pos) {
    return (field & bitboard(1) << pos);
}

constexpr uint8_t Pieces::count1(bitboard field) {
    return std::popcount(field);
}

/**
 * bitScanForward
 * @author Kim Walisch (2012)
 * @param bb bitboard to scan
 * @return index (0..63) of least significant one bit
 */
constexpr uint8_t Pieces::bsf(bitboard bb) {
    return BitScan[((bb ^ (bb - 1)) * bitboard(0x03f79d71b4cb0a89)) >> 58];
}

/**
 * bitScanReverse
 * @authors Kim Walisch, Mark Dickinson
 * @param bb bitboard to scan
 * @return index (0..63) of most significant one bit
 */
constexpr uint8_t Pieces::bsr(bitboard bb) {
    bb |= bb >> 1;
    bb |= bb >> 2;
    bb |= bb >> 4;
    bb |= bb >> 8;
    bb |= bb >> 16;
    bb |= bb >> 32;
    return BitScan[(bb * bitboard(0x03f79d71b4cb0a89)) >> 58];
}


// example of short FEN: rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR
// start from last row. lowercase letters - black, uppercase - white
// p - pawn, r - rook, n - knight, b - bishop, k - king, q - queen
Pieces::Pieces(const std::string& short_fen) {
    uint8_t row = 7;
    uint8_t col = 0;

    for (char s : short_fen) {
        if (s == '/') {
            --row;
            col = 0;
            continue;
        } else if (isdigit(s)) {
            col += s - '0';
            continue;
        }

        // lowercase, a = 97 ascii
        // uppercase, A = 65 ascii
        // an interesting feature of ascii letters,
        // xor 32 changes the letter from uppercase to lowercase and vice versa

        uint8_t side = WHITE;
        if (s < 'a') {
            side = BLACK;
            s ^= 32;
        }

        // row << 3 == row * 8
        switch (s) {
            case 'p' : set1(s_pieces_bitboards[side][PAWN],   (row << 3) + col); break;
            case 'r' : set1(s_pieces_bitboards[side][ROOK],   (row << 3) + col); break;
            case 'n' : set1(s_pieces_bitboards[side][KNIGHT], (row << 3) + col); break;
            case 'b' : set1(s_pieces_bitboards[side][BISHOP], (row << 3) + col); break;
            case 'k' : set1(s_pieces_bitboards[side][KING],   (row << 3) + col); break;
            case 'q' : set1(s_pieces_bitboards[side][QUEEN],  (row << 3) + col); break;
            default : break;
        }
        ++col;
    }
    update_bitboards();
}

void Pieces::update_bitboards() {
    s_side[BLACK] = s_pieces_bitboards[BLACK][PAWN] |
                    s_pieces_bitboards[BLACK][ROOK] |
                    s_pieces_bitboards[BLACK][KNIGHT] |
                    s_pieces_bitboards[BLACK][BISHOP] |
                    s_pieces_bitboards[BLACK][KING] |
                    s_pieces_bitboards[BLACK][QUEEN];

    s_side[WHITE] = s_pieces_bitboards[WHITE][PAWN] |
                    s_pieces_bitboards[WHITE][ROOK] |
                    s_pieces_bitboards[WHITE][KNIGHT] |
                    s_pieces_bitboards[WHITE][BISHOP] |
                    s_pieces_bitboards[WHITE][KING] |
                    s_pieces_bitboards[WHITE][QUEEN];

    s_inverse_side[BLACK] = ~s_side[BLACK];
    s_inverse_side[WHITE] = ~s_side[WHITE];

    s_all = s_side[BLACK] | s_side[WHITE];
    s_empty = ~s_all;
}

void Pieces::print() const {
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 6; ++j)
            ::print(s_pieces_bitboards[i][j]);

}