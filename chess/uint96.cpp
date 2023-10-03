#include "uint96.hpp"

void uint96::operator^= (const uint96 &object) {
    first  ^= object.first;
    second ^= object.second;
}

bool operator== (const uint96 &left, const uint96 &right) {
    return left.first == right.first && left.second == right.second;
}