#ifndef ATENIKA_MVV_LVA_HPP
#define ATENIKA_MVV_LVA_HPP

#include "defs.hpp"

/*
               Attackers
           P  N  B  R  Q  K
Victims |------------------
    P   |  5  4  3  2  1  0
    N   | 11 10  9  8  7  6
    B   | 17 16 15 14 13 12
    R   | 23 22 21 20 19 18
    Q   | 29 28 27 26 25 24
*/

// Most Valuable Victim, Least Valuable Attacker
namespace MvvLva {
    constexpr int32_t CAPTURE_BONUS = 400'000;
    constexpr int32_t PROMOTION_BONUS = 300'000;

    extern int32_t mvv_lva[PIECE_SIZE-1][PIECE_SIZE];
    void init();
};

#endif //ATENIKA_MVV_LVA_HPP
