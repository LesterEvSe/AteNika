#include "uint96.hpp"

void uint96::operator^=(const uint96 &object) {
    m_first  ^= object.m_first;
    m_second ^= object.m_second;
}

bool operator==(const uint96 &left, const uint96 &right) {
//    return m_first == obj.m_first && m_second == obj.m_second;
    return left.m_first == right.m_first && left.m_second == right.m_second;
}

uint96::operator std::bitset<96>() const {
    return (std::bitset<96>(m_first) << 64) | std::bitset<96>(m_second);
}