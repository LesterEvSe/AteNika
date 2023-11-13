#ifndef ATENICA_UINT96_HPP
#define ATENICA_UINT96_HPP

#include <cstdint>
#include <bitset>

using bits96 = std::bitset<96>;

class uint96 {
private:
    uint64_t m_first;
    uint32_t m_second;

public:
    uint96() = default;
    uint96(uint64_t most_significant, uint32_t least_significant) :
        m_first(most_significant), m_second(least_significant) {}

    // In that case we do not need a call chain, so nothing need to return
    void operator^=(const uint96 &object);
    friend bool operator==(const uint96 &left, const uint96 &right);

    // Required for hashing, because using a std::bitset in std::unordered_map
    explicit operator std::bitset<96>() const;
};

#endif //ATENICA_UINT96_HPP
