#ifndef CHESSAI_MOVE_ARRAY_HPP
#define CHESSAI_MOVE_ARRAY_HPP

#include "move.hpp"
#include "pieces.hpp"

class MoveArray {
private:
    Move m_moves[218];
    uint8_t m_size;

public:
    MoveArray() : m_moves{}, m_size{} {}
    void emplace_back(const Move &move);
    [[nodiscard]] uint8_t size() const;

    Move &operator[](uint8_t ind);
    Move  operator[](uint8_t ind) const;

};


#endif //CHESSAI_MOVE_ARRAY_HPP
