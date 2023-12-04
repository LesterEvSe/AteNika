#include "move.hpp"

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

bool operator<(const Move &left, const Move &right) {
    return left.m_score < right.m_score;
}

bool operator==(const Move &left, const Move &right) {
    return left.m_move_repr == right.m_move_repr;
}