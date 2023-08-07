#include "Pieces.hpp"
#include "ZobristHash.hpp"

int main() {
    ZobristHash::init();
    bitboard temp = 123'512;

//    std::cout << "lsb " << (unsigned)lsb(temp) << std::endl;
//    std::cout << "msb " << (unsigned)msb(temp) << std::endl;
//    printbb(temp);

//    std::string str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
//    auto pieces = Pieces(str);
//    std::cout << pieces;

    return 0;
}
