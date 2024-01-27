#include "order_info.hpp"
#include <cstring>

OrderInfo::OrderInfo() : m_ply(0) {
    std::memset(m_history, 0, sizeof(m_history));
}

void OrderInfo::operator++() { ++m_ply; }
void OrderInfo::operator--() { --m_ply; }

void OrderInfo::update_killers(Move move) {
    m_killer2[m_ply] = m_killer1[m_ply];
    m_killer1[m_ply] = move;
}
void OrderInfo::inc_history(Color color, uint8_t from, uint8_t to, int16_t depth) {
    m_history[color][from][to] += depth * depth;
}

Move OrderInfo::get_killer1() const { return m_killer1[m_ply]; }
Move OrderInfo::get_killer2() const { return m_killer2[m_ply]; }

int32_t OrderInfo::get_history(Color color, uint8_t from, uint8_t to) const {
    return m_history[color][from][to];
}