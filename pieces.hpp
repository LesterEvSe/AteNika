#ifndef CHESSAI_PIECES_HPP
#define CHESSAI_PIECES_HPP

#include "names.hpp"

// NEED TO DELETE LATER
#include <bitset>
#include <iostream>


class APiece
{
protected:
    bool m_color {false};
    bitboard m_piece {0};

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

    virtual bitboard get_attack() = 0;
};

class Pawn : public APiece
{
public:
    explicit Pawn(bool _color);
    bitboard get_moves();
    bitboard get_attack() override;
};

class King : public APiece
{
public:
    explicit King(bool _color);
    bitboard get_attack() override;
};

class Knight : public APiece
{
public:
    explicit Knight(bool _color);
    bitboard get_attack() override;
};

class Bishop : public APiece
{
public:
    explicit Bishop(bool _color);
    bitboard get_attack() override;
};

class Rook : public APiece
{
public:
    explicit Rook(bool _color);
    bitboard get_attack() override;
};

class Queen : public APiece
{
public:
    explicit Queen(bool _color);
    bitboard get_attack() override;
};



#endif //CHESSAI_PIECES_HPP
