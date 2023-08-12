#include "move_array.hpp"

void MoveArray::emplace_back(const Move &move) { m_moves[m_size++] = move; }
uint8_t MoveArray::size() const { return m_size; }

Move &MoveArray::operator[](uint8_t ind) { return m_moves[ind]; }
Move MoveArray::operator[](uint8_t ind) const { return m_moves[ind]; }
