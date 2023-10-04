#include "rays.hpp"
#include "board.hpp"
#include "movepicker.hpp"

int main() {
    Rays::init();
    Attacks::init(); // Must be init_pawn_attacks after Rays
    MovePicker::init();
    ZobristHash::init();

    bitboard blockers = 0x0004120843280816;
    uint8_t cell = 28; // 4E


    // Accept
//    bitboard white_pawns = 0xFF00; //0x807F00;
//    bitboard black_pawns = 0xBF400000000000;
//    printbb(black_pawns);
//
//    printbb(blockers);
//    printbb(Pawn::get_moves(black_pawns, blockers, BLACK));

    // Accept
//    printbb(blockers);
//    printbb(Attacks::get_queen_attacks(cell, blockers));

    return 0;
}
