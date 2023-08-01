#include "pieces.hpp"
#include <cctype> // for isdigit function

bitboard Pieces::ROWS[DIM] {0};
bitboard Pieces::INVERT_ROWS[DIM] {0};

bitboard Pieces::COLS[DIM] {0};
bitboard Pieces::INVERT_COLS[DIM] {0};

void Pieces::init_rows_cols() {
    bitboard row = 0, col = 0;
    for (int i = 0; i < DIM; ++i) {
        row |= bitboard(1) << i;
        col |= bitboard(1) << (i * DIM);
    }

    for (int i = 0; i < DIM; ++i) {
        Pieces::ROWS[i] = row;
        Pieces::INVERT_ROWS[i] = ~row;

        Pieces::COLS[i] = col;
        Pieces::INVERT_COLS[i] = ~col;

        row <<= DIM;
        col <<= 1;
    }
}


// example of short FEN: rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR
// start from last row. lowercase letters - black, uppercase - white
// p - pawn, r - rook, n - knight, b - bishop, k - king, q - queen
Pieces::Pieces(const std::string& short_fen) {
    Pieces::init_rows_cols();
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
        uint8_t curr_side = BLACK;
        if (s < 'a') {
            curr_side = WHITE;
            s ^= 32;
        }

        // (row << 3) == row * 8
        switch (s) {
            case 'p' : set1(m_pieces_bitboards[curr_side][PAWN], (row << 3) + col); break;
            case 'r' : set1(m_pieces_bitboards[curr_side][ROOK], (row << 3) + col); break;
            case 'n' : set1(m_pieces_bitboards[curr_side][KNIGHT], (row << 3) + col); break;
            case 'b' : set1(m_pieces_bitboards[curr_side][BISHOP], (row << 3) + col); break;
            case 'k' : set1(m_pieces_bitboards[curr_side][KING], (row << 3) + col); break;
            case 'q' : set1(m_pieces_bitboards[curr_side][QUEEN], (row << 3) + col); break;
            default : break;
        }
        ++col;
    }
    update_bitboards();
}

void Pieces::update_bitboards() {
    m_side[BLACK] = m_pieces_bitboards[BLACK][PAWN] |
                    m_pieces_bitboards[BLACK][ROOK] |
                    m_pieces_bitboards[BLACK][KNIGHT] |
                    m_pieces_bitboards[BLACK][BISHOP] |
                    m_pieces_bitboards[BLACK][KING] |
                    m_pieces_bitboards[BLACK][QUEEN];

    m_side[WHITE] = m_pieces_bitboards[WHITE][PAWN] |
                    m_pieces_bitboards[WHITE][ROOK] |
                    m_pieces_bitboards[WHITE][KNIGHT] |
                    m_pieces_bitboards[WHITE][BISHOP] |
                    m_pieces_bitboards[WHITE][KING] |
                    m_pieces_bitboards[WHITE][QUEEN];

    inverse_side[BLACK] = ~m_side[BLACK];
    inverse_side[WHITE] = ~m_side[WHITE];

    m_all = m_side[BLACK] | m_side[WHITE];
    m_empty = ~m_all;
}

bool operator==(const Pieces& left, const Pieces& right) {
    for (uint8_t i = 0; i < 2; ++i)
        for (uint8_t j = 0; j < 6; ++j)
            if (left.m_pieces_bitboards[i][j] != right.m_pieces_bitboards[i][j])
                return false;
    return true;
}

std::ostream& operator<<(std::ostream& out, const Pieces& pieces) {
    out << "   ";
    for (char let = 'A'; let <= 'H'; ++let)
        out << ' ' << let;
    out << std::endl;

    for (int8_t row = 7; row >= 0; --row) {
        out << row+1 << " |";
        for (uint8_t col = 0; col < Pieces::DIM; ++col) {
            if      (get_bit(pieces.m_pieces_bitboards[BLACK][PAWN], row * 8 + col)) out << " p";
            else if (get_bit(pieces.m_pieces_bitboards[BLACK][ROOK], row * 8 + col)) out << " r";
            else if (get_bit(pieces.m_pieces_bitboards[BLACK][KNIGHT], row * 8 + col)) out << " n";
            else if (get_bit(pieces.m_pieces_bitboards[BLACK][BISHOP], row * 8 + col)) out << " b";
            else if (get_bit(pieces.m_pieces_bitboards[BLACK][KING], row * 8 + col)) out << " k";
            else if (get_bit(pieces.m_pieces_bitboards[BLACK][QUEEN], row * 8 + col)) out << " q";

            else if (get_bit(pieces.m_pieces_bitboards[WHITE][PAWN], row * 8 + col)) out << " P";
            else if (get_bit(pieces.m_pieces_bitboards[WHITE][ROOK], row * 8 + col)) out << " R";
            else if (get_bit(pieces.m_pieces_bitboards[WHITE][KNIGHT], row * 8 + col)) out << " N";
            else if (get_bit(pieces.m_pieces_bitboards[WHITE][BISHOP], row * 8 + col)) out << " B";
            else if (get_bit(pieces.m_pieces_bitboards[WHITE][KING], row * 8 + col)) out << " K";
            else if (get_bit(pieces.m_pieces_bitboards[WHITE][QUEEN], row * 8 + col)) out << " Q";
            else out << " -";
        }
        out << std::endl;
    }
    out << "--------------------\n\n";
    return out;
}