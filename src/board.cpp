#include "board.hpp"
#include <sstream> // for std::istringstream in constructor

// example of short FEN: rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w Kq - 4
// start from last row. lowercase letters - black, uppercase - white
// p - pawn, r - rook, n - knight, b - bishop, k - king, q - queen
Board::Board(std::string short_fen) {
    std::istringstream iss(short_fen);
    std::string pieces;
    iss >> pieces;

    int8_t pos = 56;
    for (uint16_t i = 0; i < pieces.size(); ++i, ++pos)
        switch (pieces[i]) {
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

            case '/' : pos -= 17; break; // Because we have pos increment at the end
            default  : pos += static_cast<int8_t>(pieces[i] - '1'); // Because of increment too
        }

    char move;
    iss >> move;
    m_player_move = move == 'w' ? WHITE : BLACK;

    std::string castling;
    iss >> castling;
    for (uint8_t i = 0; i < castling.size(); ++i)
        switch (castling[i]) {
            case 'K' : m_castling_rights |= 1; break;
            case 'Q' : m_castling_rights |= 2; break;
            case 'k' : m_castling_rights |= 4; break;
            case 'q' : m_castling_rights |= 8; break;
            default  : break;
        }

    std::string en_passant;
    iss >> en_passant;
    if (en_passant == "-")
        m_en_passant_cell = ZERO;
    else
        m_en_passant_cell = ONE << get_cell(en_passant);

    iss >> m_half_moves_counter;

    update_bitboards();
    m_hash.set_hash(*this); // Order is important! Hash need after all initializations
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

Color Board::get_curr_player_move()     const { return m_player_move; }
Color Board::get_opponent_player_move() const { return m_player_move == WHITE ? BLACK : WHITE; }

bitboard Board::get_pieces(Color color, PieceType piece) const { return m_pieces[color][piece]; }
bitboard Board::get_side_pieces(Color color)             const { return m_side[color]; }
bitboard Board::get_all_pieces()  const { return m_all;  }
bitboard Board::get_free_cells()  const { return ~m_all; }

uint8_t Board::get_half_moves()   const { return m_half_moves_counter;  }
uint8_t Board::get_en_passant()   const { return m_en_passant_cell;     }

bool Board::get_white_ks_castle() const { return m_castling_rights & 1; }
bool Board::get_white_qs_castle() const { return m_castling_rights & 2; }
bool Board::get_black_ks_castle() const { return m_castling_rights & 4; }
bool Board::get_black_qs_castle() const { return m_castling_rights & 8; }


PieceType Board::get_piece_at(Color color, uint8_t index) const {
    bitboard field = ONE << index;

    if (field & m_pieces[color][PAWN])   return PAWN;
    if (field & m_pieces[color][KNIGHT]) return KNIGHT;
    if (field & m_pieces[color][BISHOP]) return BISHOP;
    if (field & m_pieces[color][ROOK])   return ROOK;
    if (field & m_pieces[color][QUEEN])  return QUEEN;
    if (field & m_pieces[color][KING])   return KING;

    std::string col = color == WHITE ? "white" : "black";
    error(col + " has not piece at " + FIELD[index] + " cell");
    return NONE; // Will never reach this line
}

bool Board::in_check(Color color) const {
    bitboard pieces = get_pieces(color, KING);
    uint8_t king_cell = lsb(pieces);
    return under_attack(color, king_cell);
}

// This allows us not to go through absolutely all the pieces in order
// to say whether some piece attack this cell or not.

// Main idea. If on the current square there is for example a WHITE KNIGHT,
// and it attacks BLACK KNIGHT, therefore the cell under attack.
bool Board::under_attack(Color defender, uint8_t cell) const {
    Color attacker = get_opposite_move(defender);
    if (get_pieces(attacker, PAWN) & Attacks::get_pawn_attacks(defender, cell)) return true;
    if (get_pieces(attacker, KNIGHT) & Attacks::get_knight_attacks(cell)) return true;
    if (get_pieces(attacker, KING) & Attacks::get_king_attacks(cell)) return true;

    // Sliding Attacks
    bitboard blockers = get_all_pieces();
    if ((get_pieces(attacker, ROOK) | get_pieces(attacker, QUEEN)) &
        Attacks::get_rook_attacks(cell, blockers)) return true;

    if ((get_pieces(attacker, BISHOP) | get_pieces(attacker, QUEEN)) &
        Attacks::get_bishop_attacks(cell, blockers)) return true;
    return false;
}

void Board::add_piece(Color color, PieceType piece, uint8_t cell) {
    set1(m_pieces[color][piece], cell);
    m_hash.xor_piece(color, piece, cell);
}

void Board::remove_piece(Color color, PieceType piece, uint8_t cell) {
    set0(m_pieces[color][piece], cell);
    m_hash.xor_piece(color, piece, cell);
}

std::ostream& operator<<(std::ostream &out, const Board &pieces) {
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
