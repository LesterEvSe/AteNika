#ifndef ATENICA_RAYS_HPP
#define ATENICA_RAYS_HPP

#include "defs.hpp"

namespace Rays {
namespace hidden {
    // 8 sides of a src piece
    extern bitboard _rays[8][64];

    // The next 2 functions are auxiliary
    bitboard _west_shift(bitboard diag, uint8_t num);
    bitboard _east_shift(bitboard diag, uint8_t num);
} // hidden

    /**
    *    N
    *  W   E
    *    S
    */
    enum Direction : uint8_t {
        NORTH = 0,
        SOUTH = 1,
        EAST  = 2,
        WEST  = 3,

        NORTH_EAST = 4,
        NORTH_WEST = 5,
        SOUTH_EAST = 6,
        SOUTH_WEST = 7
    };

    void init();
    bitboard get_ray(Direction dir, uint8_t cell);
}

#endif //ATENICA_RAYS_HPP
