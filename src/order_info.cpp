#include "order_info.hpp"
#include <cstring> // for std::memset

OrderInfo::OrderInfo() : m_ply(0)
{
    std::memset(m_killers1, 0, sizeof(m_killers1));
    std::memset(m_killers2, 0, sizeof(m_killers2));
}

void OrderInfo::operator++() { ++m_ply; }
void OrderInfo::operator--() { --m_ply; }

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

int16_t OrderInfo::get_ply() const {
    return m_ply;
}