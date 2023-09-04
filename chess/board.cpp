#include "board.hpp"

// example of short FEN: rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR
// start from last row. lowercase letters - black, uppercase - white
// p - pawn, r - rook, n - knight, b - bishop, k - king, q - queen
Board::Board(std::string short_fen) {
    int8_t pos = 56;
    for (uint8_t i = 0; i < short_fen.size(); ++i, ++pos)
        switch (short_fen[i]) {
            case 'p' : m_pieces[BLACK][PAWN]   |= (ONE << pos); break;
            case 'r' : m_pieces[BLACK][ROOK]   |= (ONE << pos); break;
            case 'n' : m_pieces[BLACK][KNIGHT] |= (ONE << pos); break;
            case 'b' : m_pieces[BLACK][BISHOP] |= (ONE << pos); break;
            case 'k' : m_pieces[BLACK][KING]   |= (ONE << pos); break;
            case 'q' : m_pieces[BLACK][QUEEN]  |= (ONE << pos); break;

            case 'P' : m_pieces[WHITE][PAWN]   |= (ONE << pos); break;
            case 'R' : m_pieces[WHITE][ROOK]   |= (ONE << pos); break;
            case 'N' : m_pieces[WHITE][KNIGHT] |= (ONE << pos); break;
            case 'B' : m_pieces[WHITE][BISHOP] |= (ONE << pos); break;
            case 'K' : m_pieces[WHITE][KING]   |= (ONE << pos); break;
            case 'Q' : m_pieces[WHITE][QUEEN]  |= (ONE << pos); break;

            case '/' : pos -= 17; break; // Because we have increment at the end
            default  : pos += static_cast<int8_t>(short_fen[i] - '1'); // Because of increment too
        }
    update_bitboards();
}

void Board::update_bitboards() {
    m_side[WHITE] = m_pieces[WHITE][PAWN] |
                    m_pieces[WHITE][ROOK] |
                    m_pieces[WHITE][KNIGHT] |
                    m_pieces[WHITE][BISHOP] |
                    m_pieces[WHITE][KING] |
                    m_pieces[WHITE][QUEEN];

    m_side[BLACK] = m_pieces[BLACK][PAWN] |
                    m_pieces[BLACK][ROOK] |
                    m_pieces[BLACK][KNIGHT] |
                    m_pieces[BLACK][BISHOP] |
                    m_pieces[BLACK][KING] |
                    m_pieces[BLACK][QUEEN];

    m_all = m_side[WHITE] | m_side[BLACK];
}

std::ostream& operator<<(std::ostream& out, const Board& pieces) {
    out << "   ";
    for (char let = 'A'; let <= 'H'; ++let)
        out << ' ' << let;
    out << std::endl;

    for (int8_t row = 7; row >= 0; --row) {
        out << row+1 << " |";
        for (uint8_t col = 0; col < 8; ++col) {
            int8_t temp = row * 8 + col;
            if      (pieces.m_pieces[BLACK][PAWN]   & (ONE << temp)) out << " p";
            else if (pieces.m_pieces[BLACK][ROOK]   & (ONE << temp)) out << " r";
            else if (pieces.m_pieces[BLACK][KNIGHT] & (ONE << temp)) out << " n";
            else if (pieces.m_pieces[BLACK][BISHOP] & (ONE << temp)) out << " b";
            else if (pieces.m_pieces[BLACK][KING]   & (ONE << temp)) out << " k";
            else if (pieces.m_pieces[BLACK][QUEEN]  & (ONE << temp)) out << " q";

            else if (pieces.m_pieces[WHITE][PAWN]   & (ONE << temp)) out << " P";
            else if (pieces.m_pieces[WHITE][ROOK]   & (ONE << temp)) out << " R";
            else if (pieces.m_pieces[WHITE][KNIGHT] & (ONE << temp)) out << " N";
            else if (pieces.m_pieces[WHITE][BISHOP] & (ONE << temp)) out << " B";
            else if (pieces.m_pieces[WHITE][KING]   & (ONE << temp)) out << " K";
            else if (pieces.m_pieces[WHITE][QUEEN]  & (ONE << temp)) out << " Q";
            else out << " -";
        }
        out << std::endl;
    }
    out << "--------------------\n\n";
    return out;
}
