#include "pieces.hpp"
#include <random>
#include "ZobristHash.hpp"

int main() {
    ZobristHash::init();
    bitboard temp = 123'512;

//    std::cout << "lsb " << (unsigned) Pieces::lsb(temp) << std::endl;
//    std::cout << "msb " << (unsigned) Pieces::msb(temp) << std::endl;

    print(temp);
    std::string str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

    return 0;
}
