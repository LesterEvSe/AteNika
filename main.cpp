#include "pieces.hpp"
#include <random>
#include "ZobristHash.hpp"


int main() {
    bitboard temp = 123'512;
//    ZobristHash hash = ZobristHash::get_instance();
//    std::cout << hash(4, 1, 3);

    std::string str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    auto pieces = Pieces(str);
    // std::cout << pieces << pieces;

//    Pieces::set0(temp, 4);
//    print(temp);
//    std::cout << unsigned(Pieces::bsf(temp)) << ' ' << unsigned(Pieces::bsr(temp));

    return 0;
}
