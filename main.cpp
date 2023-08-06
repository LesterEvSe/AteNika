#include "Pieces.hpp"
#include "ZobristHash.hpp"

#include <array>

int main() {
    ZobristHash::init();
    bitboard temp = 123'512;
    std::array<bitboard, 64> smth{5};
    std::cout << smth[60];



//    std::cout << "lsb " << (unsigned) Pieces::lsb(temp) << std::endl;
//    std::cout << "msb " << (unsigned) Pieces::msb(temp) << std::endl;

//    print(temp);
    std::string str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    auto pieces = Pieces(str);
    std::cout << pieces;

    return 0;
}
