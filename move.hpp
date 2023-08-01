#ifndef CHESSAI_MOVE_HPP
#define CHESSAI_MOVE_HPP

#include <cstdint>

enum class Flag : uint8_t {
    DEFAULT = 0,

    PAWN_LONG_MOVE = 1,
    EN_PASSANT_CAPTURE = 2, // Taken on the pass, from the French.

    WS_CASTLING = 3,
    WL_CASTLING = 4,

    BS_CASTLING = 5,
    BL_CASTLING = 6,

    PROMOTE_TO_KNIGHT = 7,
    PROMOTE_TO_BISHOP = 8,
    PROMOTE_TO_ROOK = 9,
    PROMOTE_TO_QUEEN = 10
};

struct Move {
    uint8_t m_from {0};
    uint8_t m_to {0};

    uint8_t m_attacker_type {0};
    uint8_t m_attacker_side {0};

    uint8_t m_defender_type {0};
    uint8_t m_defender_side {0};

    Flag m_flag {Flag::DEFAULT};

};


#endif //CHESSAI_MOVE_HPP
