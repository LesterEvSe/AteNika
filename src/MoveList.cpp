#include "MoveList.hpp"

uint8_t MoveList::size() const {
    return m_size;
}

Move &MoveList::operator[](uint8_t index) {
    return m_moves[index];
}