#include "move_list.hpp"

uint8_t MoveList::size() const {
    return m_size;
}

Move &MoveList::operator[](uint8_t index) {
    return m_moves[index];
}

void MoveList::emplace_back(const Move &move) {
    m_moves[m_size++] = move;
}