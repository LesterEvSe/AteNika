#ifndef CHESSAI_RAYS_HPP
#define CHESSAI_RAYS_HPP

#include "bitboard.hpp"

enum Direction : uint8_t {
    NORTH = 0,
    SOUTH = 1,
    EAST  = 2,
    WEST  = 3,

    NORTH_EAST = 4,
    NORTH_WEST = 5,
    SOUTH_EAST = 6,
    SOUTH_WEST = 7,
};

/**
 *    N
 *  W   E
 *    S
 */

class Rays {
private:
    // 8 sides of a chess piece
    static uint64_t s_rays[8][64];

    // The next 2 functions are auxiliary
    static uint64_t east_shift(uint64_t diag, uint8_t n);
    static uint64_t west_shift(uint64_t diag, uint8_t n);
    static void calculate_rays();

public:
    static void init();
    static uint64_t get_ray(Direction side, uint8_t cell);
};


#endif //CHESSAI_RAYS_HPP
