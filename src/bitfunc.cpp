#include "bitfunc.hpp"

uint8_t memo_bits[1 << 16];

void init_bits_pre_calculation() {
    for (uint32_t i = 0; i < (1 << 16); ++i)
        memo_bits[i] = memo_bits[i >> 1] + (i & 1);
}