#include "uint96.hpp"
#include <iostream>

void uint96::operator^=(const uint96 &object) {
    m_first  ^= object.m_first;
    m_second ^= object.m_second;
}

bool operator==(const uint96 &left, const uint96 &right) {
    return left.m_first == right.m_first && left.m_second == right.m_second;
}

std::ostream &operator<<(std::ostream &out, const uint96 &num) {
    out << std::hex << std::uppercase << num.m_first << num.m_second;
    return out;
}

uint96::operator std::bitset<96>() const {
    return (std::bitset<96>(m_first) << 64) | std::bitset<96>(m_second);
}