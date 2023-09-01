#ifndef CHESSAI_BOARD_HPP
#define CHESSAI_BOARD_HPP

#include "defs.hpp"
#include "bitfunc.hpp"

class Board {
private:

    // The top 18 bitboards to work
    // m_all pieces, black and white (black pawns, white bishops ...)
    bitboard m_pieces[2][6] {0};

    // white and black pieces
    bitboard m_side[2] {0};
    bitboard m_inverse_side[2] {0};

    // m_all the pieces on the board
    bitboard m_all {0};
    bitboard m_empty {0};

public:
    Board();

    // using short FEN (Forsyth-Edwards Notation):
    // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
    explicit Board(const std::string& short_fen);
    void update_bitboards();
    friend std::ostream& operator<<(std::ostream& out, const Board& board);
};


#endif //CHESSAI_BOARD_HPP
