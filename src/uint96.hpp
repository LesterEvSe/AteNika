#ifndef ATENICA_UINT96_HPP
#define ATENICA_UINT96_HPP

#include <cstdint>

struct uint96 {
    uint64_t first;
    uint32_t second;

    // In that case we do not need a call chain, so nothing need to return
    void operator^=(const uint96 &object);
    friend bool operator==(const uint96 &left, const uint96 &right);
};

#endif //ATENICA_UINT96_HPP
