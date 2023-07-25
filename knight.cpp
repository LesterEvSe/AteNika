#include "pieces.hpp"

Knight::Knight(bool _color) {
    m_color = _color;
    switch (m_color) {
        case WHITE:
            m_piece = (1 << 1) | (1 << 6);
            break;
        default:
            m_piece = ((bitboard) 1 << 57) | ((bitboard) 1 << 62);
    }
}

bitboard Knight::get_attack() {
    bitboard lr1 = (m_piece >> 1) & (~Column::H) |
              (m_piece << 1) & (~Column::A);

    bitboard lr2 = (m_piece >> 2) & ~(Column::H | Column::G) |
              (m_piece << 2) & ~(Column::A | Column::B);
    return lr1 << 16 | lr1 >> 16 | lr2 << 8 | lr2 >> 8;
}