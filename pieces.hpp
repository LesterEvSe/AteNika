#ifndef CHESSAI_PIECES_HPP
#define CHESSAI_PIECES_HPP

#include "names.hpp"

// NEED TO DELETE LATER
#include <bitset>
#include <iostream>

/**
 *      A   B   C   D   E   F   G   H
 * 8 | 56  57  58  59  60  61  62  63
 * 7 | 48  49  50  51  52  53  54  55
 * 6 | 40  41  42  43  44  45  46  47
 * 5 | 32  33  34  35  36  37  38  39
 * 4 | 24  25  26  27  28  29  30  31
 * 3 | 16  17  18  19  20  21  22  23
 * 2 |  8   9  10  11  12  13  14  15
 * 1 |  0   1   2   3   4   5   6   7
 */


class APiece
{
protected:
    bool m_color {false};
    u64 m_piece  {0};

public:
    // for Debug
    friend std::ostream& operator<< (std::ostream& out, const APiece& piece) {
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

    virtual u64 get_attack() = 0;
};

class Pawn : public APiece
{
public:
    explicit Pawn(bool _color);
    u64 get_moves();
    u64 get_attack() override;
};

class King : public APiece
{
public:
    explicit King(bool _color);
    u64 get_attack() override;
};

class Knight : public APiece
{
public:
    explicit Knight(bool _color);
    u64 get_attack() override;
};

class Bishop : public APiece
{
public:
    explicit Bishop(bool _color);
    u64 get_attack() override;
};

class Rook : public APiece
{
public:
    explicit Rook(bool _color);
    u64 get_attack() override;
};

class Queen : public APiece
{
public:
    explicit Queen(bool _color);
    u64 get_attack() override;
};



#endif //CHESSAI_PIECES_HPP
