#ifndef ATENIKA_MVV_LVA_HPP
#define ATENIKA_MVV_LVA_HPP

#include "defs.hpp"

namespace MvvLva {
    // MVV - Most Valuable Victim
    // LVA - Least Valuable Attacker
    extern uint8_t mvv_lva[PIECE_SIZE - 1][PIECE_SIZE];

    constexpr int32_t CAPTURE_BONUS = 2000;
    constexpr int32_t PROMOTION_BONUS = 1000;

    void init();
}

#endif //ATENIKA_MVV_LVA_HPP
