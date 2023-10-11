#ifndef ATENICA_MOVE_HPP
#define ATENICA_MOVE_HPP

#include "defs.hpp"

class Move {
public:
    enum Flag : uint8_t {
        QUIET = 0,
        CAPTURE = 1,
        LONG_PAWN_MOVE = 2,
        QSIDE_CASTLING = 3,
        KSIDE_CASTLING = 4,
        EN_PASSANT = 5,
        PROMOTION = 6,
        CAPTURE_PROMOTION = 7 // if Capture 001 and Promotion 110 bits set
    };

private:
    // Move representation
    struct {
        uint8_t m_from : 6;
        uint8_t m_to   : 6;

        Flag    m_flag : 3;
        PieceType m_move_piece      : 3;
        PieceType m_captured_piece  : 3;
        PieceType m_promotion_piece : 3;
    };

    int32_t m_score;

public:
    Move() = default;
    Move(uint8_t from,
         uint8_t to,
         PieceType move_piece,
         Flag flag = QUIET,
         PieceType captured_piece = NONE,
         PieceType promotion_piece = NONE
    ) :
        m_from(from),
        m_to(to),
        m_move_piece(move_piece),
        m_flag(flag),
        m_captured_piece(captured_piece),
        m_promotion_piece(promotion_piece),
        m_score(0) {}

    // For insertion sort in MovePicker class
    friend bool operator<(const Move &left, const Move &right);
    [[nodiscard]] int32_t get_score() const;
};

#endif //ATENICA_MOVE_HPP
