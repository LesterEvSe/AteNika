#include "order_info.hpp"
#include <cstring> // for std::memset

// killers will be initialized automatically
OrderInfo::OrderInfo() : m_ply(0) {
    std::memset(m_history, 0, sizeof(m_history));
}

void OrderInfo::operator++() { ++m_ply; }
void OrderInfo::operator--() { --m_ply; }

void OrderInfo::add_history(uint8_t from, uint8_t to, int16_t depth) {
    m_history[from][to] += depth;
}

void OrderInfo::add_killer(Move move) {
    m_killers2[m_ply] = m_killers1[m_ply];
    m_killers1[m_ply] = move;
}


Move OrderInfo::get_killer1() const {
    return m_killers1[m_ply];
}
Move OrderInfo::get_killer2() const {
    return m_killers2[m_ply];
}

int32_t OrderInfo::get_history(uint8_t from, uint8_t to) const {
    return m_history[from][to];
}
int16_t OrderInfo::get_ply() const {
    return m_ply;
}