#include "pieces.hpp"

Knight::Knight(bool _color) {
    m_color = _color;
    switch (m_color) {
        case WHITE:
            m_piece |= (1 << 1) | (1 << 6);
            break;
        default:
            m_piece |= ((u64) 1 << 57) | ((u64) 1 << 62);
    }
}

u64 Knight::get_attack() {
    return 0;
}