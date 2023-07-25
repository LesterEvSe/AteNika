#include "pieces.hpp"

King::King(bool _color) {
    m_color = _color;
    switch (m_color) {
        case WHITE:
            m_piece = (1 << 4);
            break;
        default:
            m_piece = ((bitboard)1 << 60);
    }
}

bitboard King::get_attack() {
    return ((m_piece << 7) | (m_piece >> 1) | (m_piece >> 9)) & ~Column::H |
            (m_piece << 8) | (m_piece >> 8) |
           ((m_piece << 9) | (m_piece << 1) | (m_piece >> 7)) & ~Column::A;
}