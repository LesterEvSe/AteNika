#ifndef ATENIKA_MOVE_HPP
#define ATENIKA_MOVE_HPP

#include <iostream> // for std::ostream
#include "defs.hpp"

class Board;

// used only 4 bytes (I want 3 bytes, but alignment...)
class Move {
public:
    enum Flag : uint8_t {
        QUIET = 0,
        LONG_PAWN_MOVE = 2,
        QSIDE_CASTLING = 3,
        KSIDE_CASTLING = 4,
        EN_PASSANT = 5,
        CAPTURE = 8,
        PROMOTION = 16,
        CAPTURE_PROMOTION = 24 // if Capture 1000 and Promotion 10000 bits set
    };

private:
    // Move representation
    union {
        struct {
            uint8_t m_from: 6;
            uint8_t m_to: 6;

            Flag m_flag: 5;
            PieceType m_move_piece: 3;
            PieceType m_captured_piece: 3;
            PieceType m_promotion_piece: 3;
        };

        // For comparison
        int32_t m_move_repr;
    };

    int32_t m_score;

public:
    Move() = default;
    // example e4e5 d8c6 or something else
    explicit Move(const Board &board, const std::string &move);

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

    [[nodiscard]] uint8_t get_from_cell() const;
    [[nodiscard]] uint8_t get_to_cell() const;
    [[nodiscard]] Move::Flag get_flag() const;

    [[nodiscard]] PieceType get_move_piece() const;
    [[nodiscard]] PieceType get_captured_piece() const;
    [[nodiscard]] PieceType get_promotion_piece() const;
    [[nodiscard]] int32_t get_score() const;

    void set_score(int32_t val);
    static bool isMove(const std::string &move);

    // For selection sort in MovePicker class
    friend bool operator<(const Move &left, const Move &right);
    friend bool operator==(const Move &left, const Move &right);
    explicit operator std::string() const;
};

#endif //ATENIKA_MOVE_HPP
