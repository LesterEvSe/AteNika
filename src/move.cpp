#include "move.hpp"
#include "board.hpp"
#include "move_list.hpp"
#include "movegen.hpp"

// For example: e4e5 f3c6 a1h8 and so on
Move::Move(const Board &board, const std::string &move) {
    if (!Move::isMove(move))
       throw std::invalid_argument("Input data is incorrect");

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

bool Move::isMove(const std::string &move) {
    if (move.size() != 4) return false;

    // Basic check if such a move can exist at all
    for (int i = 0; i < 2; i += 2) {
        if (move[i] < 'a' || move[i] > 'h') return false;
        if (move[i+1] < '1' || move[i+1] > '8') return false;
    }

    return true;
}

bool operator<(const Move &left, const Move &right) {
    return left.m_score < right.m_score;
}

bool operator==(const Move &left, const Move &right) {
    return left.m_move_repr == right.m_move_repr;
}

Move::operator std::string() const {
    return FIELD[m_from] + FIELD[m_to];
}
