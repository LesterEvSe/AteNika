#ifndef CHESSAI_PIECES_HPP
#define CHESSAI_PIECES_HPP

#include "names.hpp"
#include <bit> // for std::popcount

class Piece
{
protected:
    // The method for counting is taken from the website: https://www.chessprogramming.org/BitScan
    static constexpr const uint8_t BitScan[64] = {
       0, 47,  1, 56, 48, 27,  2, 60,
       57, 49, 41, 37, 28, 16,  3, 61,
       54, 58, 35, 52, 50, 42, 21, 44,
       38, 32, 29, 23, 17, 11,  4, 62,
       46, 55, 26, 59, 40, 36, 15, 53,
       34, 51, 20, 43, 31, 22, 10, 45,
       25, 39, 14, 33, 19, 30,  9, 24,
       13, 18,  8, 12,  7,  6,  5, 63
    };

public:
    // for Debug
    /**
    friend std::ostream& operator<< (std::ostream& out, const Piece& piece) {
        std::bitset<64> bits(piece.m_piece);
        std::cout << "   ";

        for (char let = 'A'; let <= 'H'; ++let)
            std::cout << ' ' << let;
        std::cout << std::endl;

        for (int i = 0; i < 8; ++i) {
            std::cout << i+1 << " |";
            for (int j = 0; j < 8; ++j)
                std::cout << ' ' << bits[8*i + j];
            std::cout << std::endl;
        }
        std::cout << "\n--------------------\n\n";
        return out;
    }
    */

    static constexpr void set0(bitboard& field, uint8_t pos) {
        field &= ~(bitboard(1) << pos);
    }

    static constexpr void set1(bitboard& field, uint8_t pos) {
        field |= bitboard(1) << pos;
    }

    static constexpr bool get(bitboard field, uint8_t pos) {
        return (field & bitboard(1) << pos);
    }

    static constexpr uint8_t count1(bitboard field) {
        return std::popcount(field);
    }

   /**
    * bitScanForward
    * @author Kim Walisch (2012)
    * @param bb bitboard to scan
    * @return index (0..63) of least significant one bit
    */
    static constexpr uint8_t bsf(bitboard bb) {
        return BitScan[((bb ^ (bb-1)) * bitboard(0x03f79d71b4cb0a89)) >> 58];
    }

   /**
    * bitScanReverse
    * @authors Kim Walisch, Mark Dickinson
    * @param bb bitboard to scan
    * @return index (0..63) of most significant one bit
    */
    static constexpr uint8_t bsr(bitboard bb) {
        bb |= bb >> 1;
        bb |= bb >> 2;
        bb |= bb >> 4;
        bb |= bb >> 8;
        bb |= bb >> 16;
        bb |= bb >> 32;
        return BitScan[(bb * bitboard(0x03f79d71b4cb0a89)) >> 58];
    }

};



#endif //CHESSAI_PIECES_HPP
