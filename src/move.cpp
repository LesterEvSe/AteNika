#include "move.hpp"
#include "board.hpp"
#include "move_list.hpp"
#include "movegen.hpp"

Move::Move(const Board &board, const std::string &move) {
    MoveList legal_moves = Movegen(board).get_legal_moves();
    int i;

    for (i = 0; i < legal_moves.size(); ++i)
        if (move == static_cast<std::string>(legal_moves[i])) {
            *this = legal_moves[i];
            break;
        }

    if (i == legal_moves.size())
        throw std::invalid_argument("You can't make that move");
}

uint8_t Move::get_from_cell() const { return m_from; }
uint8_t Move::get_to_cell()   const { return m_to;   }
Move::Flag Move::get_flag()   const { return m_flag; }

PieceType Move::get_move_piece()      const { return m_move_piece;      }
PieceType Move::get_captured_piece()  const { return m_captured_piece;  }
PieceType Move::get_promotion_piece() const { return m_promotion_piece; }

int32_t Move::get_score() const {
    return m_score;
}
void Move::set_score(int32_t val) {
    m_score = val;
}

// Basic check if such a move can exist at all
bool Move::isMove(const std::string &move) {
    if (move.size() != 4 && move.size() != 5) return false;

    for (uint8_t i = 0; i <= 2; i += 2) {
        if (move[i] < 'a' || move[i] > 'h') return false;
        if (move[i+1] < '1' || move[i+1] > '8') return false;
    }
    return true;
}

bool operator<(const Move &left, const Move &right) {
    return left.m_score < right.m_score;
}

bool operator==(const Move &left, const Move &right) {
    return left.m_from == right.m_from &&
           left.m_to == right.m_to &&
           left.m_flag == right.m_flag &&
           left.m_move_piece == right.m_move_piece &&
           left.m_captured_piece == right.m_captured_piece &&
           left.m_promotion_piece == right.m_promotion_piece;
}

Move::operator std::string() const {
    if (m_flag == NULL_MOVE) return "NULL MOVE";
    char piece;

    switch (m_promotion_piece) {
        case KNIGHT: piece = 'n'; break;
        case BISHOP: piece = 'b'; break;
        case ROOK:   piece = 'r'; break;
        case QUEEN:  piece = 'q'; break;
        default: return FIELD[m_from] + FIELD[m_to];
    }
    return FIELD[m_from] + FIELD[m_to] + piece;
}
