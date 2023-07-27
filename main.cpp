#include "pieces.hpp"

/** Hm.. Maybe switch to 'C' language..?
 *  About 18 bitboards, I can realize them with struct or class with methods*/
int main() {
    bitboard temp = 123'512;

    std::string str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    Pieces pieces = Pieces(str);
    // std::cout << pieces << pieces;

//    Pieces::set0(temp, 4);
//    print(temp);
//    std::cout << unsigned(Pieces::bsf(temp)) << ' ' << unsigned(Pieces::bsr(temp));

    return 0;
}
