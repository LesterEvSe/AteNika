#include "uint96.hpp"

void uint96::operator^=(const uint96 &object) {
    first  ^= object.first;
    second ^= object.second;
}

bool operator==(const uint96 &left, const uint96 &right) {
//    return first == obj.first && second == obj.second;
    return left.first == right.first && left.second == right.second;
}

uint96::operator std::bitset<96>() const {
    return (std::bitset<96>(first) << 64) | std::bitset<96>(second);
}