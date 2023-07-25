#include "pieces.hpp"


Pawn::Pawn(bool _color) {
    m_color = _color;
    switch (m_color) {
        case WHITE:
            m_piece = ROW2;
            break;

        default:
            m_piece = ROW7;
    }
}

bitboard Pawn::get_moves() {
    switch (m_color) {
        case WHITE:
            return (m_piece << 8) | (m_piece & ROW2) << 16;
        default:
            return (m_piece >> 8) | (m_piece & ROW7) >> 16;
    }
}

bitboard Pawn::get_attack() {
    switch (m_color) {
        case WHITE:
            return (m_piece << 9) & (~Column::A) | (m_piece << 7) & (~Column::H);
        default:
            return (m_piece >> 9) & (~Column::H) | (m_piece >> 7) & (~Column::A);
    }
}