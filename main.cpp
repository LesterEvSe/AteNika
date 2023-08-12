#include "pieces.hpp"
#include "zobrist_hash.hpp"
#include "attacks.hpp"
#include "sliding_attacks.hpp"
#include "move_array.hpp"

int main() {
    ZobristHash::init();
    Rays::init();
    Attacks::init();
    SlAttack::init();

    MoveArray movarr = MoveArray();


/// TEST
    bitboard temp = 0x0004120843280816;
    uint8_t cell = 28;
    printbb(temp);
    // printbb(SlAttack::get_queen_captures(cell, temp));

//    std::string str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
//    auto pieces = Pieces(str);
//    std::cout << pieces;

    return 0;
}
