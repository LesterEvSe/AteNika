#ifndef CHESSAI_APIECE_HPP
#define CHESSAI_APIECE_HPP

#include "names.hpp"


class APiece
{
protected:
    bool m_color;
    u64 m_piece;

public:
    virtual u64 get_moves()  = 0;
    virtual u64 get_attack() = 0;
};

#endif //CHESSAI_APIECE_HPP
