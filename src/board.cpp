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
        m_en_passant_cell = get_cell(en_passant);

    // for implicit conversion
    int temp_ply;
    iss >> temp_ply;
    m_ply = temp_ply;

    update_bitboards();

    // Order is important!
    // m_hash and m_pst are initialized, after the rest of the Board fields are initialized
    m_hash.set_hash(*this);
    m_pst.set_scores(*this);
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

bool operator==(const Board &left, const Board &right) {
    return left.m_hash == right.m_hash;
}

Color Board::get_curr_move()      const { return m_player_move; }
Color Board::get_opponent_move()  const { return m_player_move == WHITE ? BLACK : WHITE; }

bitboard Board::get_attacks_for_cell(Color color, PieceType type, uint8_t cell) const {
    bitboard free_cells = ~m_all;
    switch (type) {
        case PAWN  : return Attacks::get_pawn_attacks(color, cell) & free_cells;
        case KNIGHT: return Attacks::get_knight_attacks(cell) & free_cells;
        case BISHOP: return Attacks::get_bishop_attacks(cell, m_all);
        case ROOK  : return Attacks::get_rook_attacks(cell, m_all);
        case QUEEN : return Attacks::get_queen_attacks(cell, m_all);
        case KING  : return Attacks::get_king_attacks(cell);
        default    : return ZERO;
    }
}
bitboard Board::get_pieces(Color color, PieceType type)  const { return m_pieces[color][type]; }
bitboard Board::get_side_pieces(Color color)             const { return m_side[color]; }
bitboard Board::get_all_pieces()  const { return m_all;  }
bitboard Board::get_free_cells()  const { return ~m_all; }

uint8_t Board::get_ply()          const { return m_ply;  }
ZobristHash Board::get_zob_hash() const { return m_hash; }
uint8_t Board::get_en_passant()   const { return m_en_passant_cell; }
int32_t Board::get_pst_score(Color color, GamePhase phase) const { return m_pst.get(color, phase); }

bool Board::get_white_ks_castle() const { return m_castling_rights & 1; }
bool Board::get_white_qs_castle() const { return m_castling_rights & 2; }
bool Board::get_black_ks_castle() const { return m_castling_rights & 4; }
bool Board::get_black_qs_castle() const { return m_castling_rights & 8; }

bool Board::can_white_ks_castle() const {
    if (!get_white_ks_castle())
        return false;

    static const bitboard empty_cells = (ONE << f1) | (ONE << g1);
    if ((get_free_cells() & empty_cells) != empty_cells)
        return false;

    // e1 - white king cell before castling and moves
    return !under_attack(WHITE, e1) &&
        !under_attack(WHITE, f1) && !under_attack(WHITE, g1);
}

bool Board::can_white_qs_castle() const {
    if (!get_white_qs_castle())
        return false;

    static const bitboard empty_cells = (ONE << b1) | (ONE << c1) | (ONE << d1);
    if ((get_free_cells() & empty_cells) != empty_cells)
        return false;

    return !under_attack(WHITE, e1) &&
        !under_attack(WHITE, c1) && !under_attack(WHITE, d1);
}

bool Board::can_black_ks_castle() const {
    if (!get_black_ks_castle())
        return false;

    static const bitboard empty_cells = (ONE << f8) | (ONE << g8);
    if ((get_free_cells() & empty_cells) != empty_cells)
        return false;

    // e8 - black king cell before castling and moves
    return !under_attack(BLACK, e8) &&
        !under_attack(BLACK, f8) && !under_attack(BLACK, g8);
}

bool Board::can_black_qs_castle() const {
    if (!get_black_qs_castle())
        return false;

    static const bitboard empty_cells = (ONE << b8) | (ONE << c8) | (ONE << d8);
    if ((get_free_cells() & empty_cells) != empty_cells)
        return false;

    return !under_attack(BLACK, e8) &&
        !under_attack(BLACK, c8) && !under_attack(BLACK, d8);
}


PieceType Board::get_piece_at(Color color, uint8_t index) const {
    bitboard field = ONE << index;

    if (field & m_pieces[color][PAWN])   return PAWN;
    if (field & m_pieces[color][KNIGHT]) return KNIGHT;
    if (field & m_pieces[color][BISHOP]) return BISHOP;
    if (field & m_pieces[color][ROOK])   return ROOK;
    if (field & m_pieces[color][QUEEN])  return QUEEN;
    if (field & m_pieces[color][KING])   return KING;

    std::string player = color == WHITE ? "white" : "black";
    error(player + " has not piece at " + FIELD[index] + " cell");
    return NONE; // Will never reach this line
}

bool Board::king_in_check(Color color) const {
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
    m_pst.add(color, piece, cell);
    set1(m_pieces[color][piece], cell);
    m_hash.xor_piece(color, piece, cell);
}

void Board::remove_piece(Color color, PieceType piece, uint8_t cell) {
    m_pst.remove(color, piece, cell);
    set0(m_pieces[color][piece], cell);
    m_hash.xor_piece(color, piece, cell);
}


void Board::make(const Move &move) {
    // En passant is available on 1 move only
    m_en_passant_cell = ZERO;
    m_hash.clear_en_passant();
    ++m_ply;

    uint8_t to = move.get_to_cell();
    remove_piece(m_player_move, move.get_move_piece(), move.get_from_cell());
    add_piece(m_player_move, move.get_move_piece(), to);

    switch (move.get_flag()) {
        case Move::CAPTURE:
            remove_piece(get_opponent_move(), move.get_captured_piece(), to);
            break;
        case Move::LONG_PAWN_MOVE:
            m_en_passant_cell = (m_player_move == WHITE) ? to-8 : to+8;
            m_hash.xor_en_passant(get_file(m_en_passant_cell));
            break;
        case Move::QSIDE_CASTLING: {
            uint8_t rook_cell = (m_player_move == WHITE) ? a1 : a8;
            remove_piece(m_player_move, ROOK, rook_cell);
            add_piece(m_player_move, ROOK, rook_cell + 3);
            break;
        }
        case Move::KSIDE_CASTLING: {
            uint8_t rook_cell = (m_player_move == WHITE) ? h1 : h8;
            remove_piece(m_player_move, ROOK, rook_cell);
            add_piece(m_player_move, ROOK, rook_cell - 2);
            break;
        }
        case Move::EN_PASSANT: {
            uint8_t captured_pawn = (m_player_move == WHITE) ? to - 8 : to + 8;
            remove_piece(get_opponent_move(), PAWN, captured_pawn);
            break;
        }
        case Move::CAPTURE_PROMOTION: // Fall through to use PROMOTION code
            remove_piece(get_opponent_move(), move.get_captured_piece(), to);
        case Move::PROMOTION:
            remove_piece(m_player_move, move.get_move_piece(), to);
            add_piece(m_player_move, move.get_promotion_piece(), to);
            break;
        default: // Move::QUIET
            m_ply = (move.get_move_piece() == PAWN) ? ZERO : m_ply;
            break;
    }

    if (move.get_flag() != Move::QUIET)
        m_ply = ZERO;

    update_bitboards();
    m_player_move = get_opponent_move();
    m_hash.xor_move();

    m_castling_rights &= CASTLING[move.get_from_cell()] & CASTLING[to];
    m_hash.update_castling_rights(m_castling_rights);
}

std::string Board::get_fen() const {
    std::string fen;

    for (int8_t row = 7; row >= 0; --row) {
        // We can use char, because symbols <= '9'
        char empty = '0';

        for (uint8_t col = 0; col < 8; ++col) {
            bitboard square = ONE << (row * 8 + col);
            char piece = 0;

            if      (m_pieces[BLACK][PAWN]   & square) piece = 'p';
            else if (m_pieces[BLACK][ROOK]   & square) piece = 'r';
            else if (m_pieces[BLACK][KNIGHT] & square) piece = 'n';
            else if (m_pieces[BLACK][BISHOP] & square) piece = 'b';
            else if (m_pieces[BLACK][KING]   & square) piece = 'k';
            else if (m_pieces[BLACK][QUEEN]  & square) piece = 'q';

            else if (m_pieces[WHITE][PAWN]   & square) piece = 'P';
            else if (m_pieces[WHITE][ROOK]   & square) piece = 'R';
            else if (m_pieces[WHITE][KNIGHT] & square) piece = 'N';
            else if (m_pieces[WHITE][BISHOP] & square) piece = 'B';
            else if (m_pieces[WHITE][KING]   & square) piece = 'K';
            else if (m_pieces[WHITE][QUEEN]  & square) piece = 'Q';
            else ++empty;

            if (!piece) continue;
            if (empty != '0') {
                fen += empty;
                empty = '0';
            }
            fen += piece;
        }

        if (empty != '0') fen += empty;
        if (row)          fen += '/';
    }

    fen += m_player_move == WHITE ? " w " : " b ";
    if (get_white_ks_castle())
        fen += 'K';
    if (get_white_qs_castle())
        fen += 'Q';
    if (get_black_ks_castle())
        fen += 'k';
    if (get_black_qs_castle())
        fen += 'q';

    // if m_castling_rights == 0
    if (fen.back() == ' ')
        fen += "- ";
    else
        fen += ' ';

    if (m_en_passant_cell)
        fen += FIELD[m_en_passant_cell] + " ";
    else
        fen += "- ";
    return fen + std::to_string(m_ply);
}

std::ostream &operator<<(std::ostream &out, const Board &board) {
    out << "   ";
    for (char let = 'A'; let <= 'H'; ++let)
        out << ' ' << let;
    out << std::endl;

    for (int8_t row = 7; row >= 0; --row) {
        out << row+1 << " |";
        for (uint8_t col = 0; col < 8; ++col) {
            int8_t temp = row * 8 + col;
            bitboard sq = ONE << temp;

            if      (board.m_pieces[BLACK][PAWN]   & sq) out << " p";
            else if (board.m_pieces[BLACK][ROOK]   & sq) out << " r";
            else if (board.m_pieces[BLACK][KNIGHT] & sq) out << " n";
            else if (board.m_pieces[BLACK][BISHOP] & sq) out << " b";
            else if (board.m_pieces[BLACK][KING]   & sq) out << " k";
            else if (board.m_pieces[BLACK][QUEEN]  & sq) out << " q";

            else if (board.m_pieces[WHITE][PAWN]   & sq) out << " P";
            else if (board.m_pieces[WHITE][ROOK]   & sq) out << " R";
            else if (board.m_pieces[WHITE][KNIGHT] & sq) out << " N";
            else if (board.m_pieces[WHITE][BISHOP] & sq) out << " B";
            else if (board.m_pieces[WHITE][KING]   & sq) out << " K";
            else if (board.m_pieces[WHITE][QUEEN]  & sq) out << " Q";
            else out << " .";
        }
        out << std::endl;
    }
    out << "\n   ";

    for (char let = 'A'; let <= 'H'; ++let)
        out << ' ' << let;
    out << std::endl << std::endl;
    return out;
}

void Board::display_all() const {
    std::cout << *this;
    std::cout << "Fen: " << get_fen() << std::endl;
    std::cout << "Key: " << m_hash.get_hash() << std::endl << std::endl;
}
