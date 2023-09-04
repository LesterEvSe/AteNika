#include "rays.hpp"
#include "bitfunc.hpp"

bitboard Rays::_rays[8][64];


bitboard Rays::_west_shift(bitboard diag, uint8_t num) {
    for (uint8_t i = 0; i < num; ++i)
        diag = (diag >> 1) & ~FILE_H;
    return diag;
}

bitboard Rays::_east_shift(bitboard diag, uint8_t num) {
    for (uint8_t i = 0; i < num; ++i)
        diag = (diag << 1) & ~FILE_A;
    return diag;
}

void Rays::init()
{
    // Generate bit diagonals
    bitboard bit_A1 = ONE;
    bitboard bit_H1 = ONE << 7;
    bitboard DIAG_A1_H8 = ZERO;
    bitboard DIAG_A8_H1 = ZERO;

    for (int8_t i = 0; i < 8; ++i) {
        DIAG_A1_H8 |= bit_A1 << (9 * i);
        DIAG_A8_H1 |= bit_H1 << (7 * i);
    }

    bitboard one = ONE, west, east;
    for (int8_t i = 0; i < 64; ++i) {
        if (i % 8 == 0) {
            west = ZERO;
            east = (RANK_1 << i) & ~FILE_A;
        }

        _rays[NORTH][i] = FILE_A << (8 + i);
        _rays[SOUTH][i] = (FILE_H & ~RANK_8) >> (63 - i);
        _rays[WEST][i] = west;
        _rays[EAST][i] = east;

        west |= one;
        one <<= 1;
        east = (east << 1) & ~FILE_A;


        _rays[NORTH_EAST][i] = _east_shift(DIAG_A1_H8 & ~RANK_1, get_col(i))     << (get_row(i) * 8);
        _rays[NORTH_WEST][i] = _west_shift(DIAG_A8_H1 & ~RANK_1, 7 - get_col(i)) << (get_row(i) * 8);
        _rays[SOUTH_EAST][i] = _east_shift(DIAG_A8_H1 & ~RANK_8, get_col(i))     >> ((7 - get_row(i)) * 8);
        _rays[SOUTH_WEST][i] = _west_shift(DIAG_A1_H8 & ~RANK_8, 7 - get_col(i)) >> ((7 - get_row(i)) * 8);
    }
}

bitboard Rays::get_ray(Rays::Direction dir, uint8_t cell) {
    return _rays[dir][cell];
}