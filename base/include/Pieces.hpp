#ifndef CHESSAI_PIECES_HPP
#define CHESSAI_PIECES_HPP

#include "Bitboard.hpp"
#include "ZobristHash.hpp"
#include <string>

class Pieces
{
    friend class ZobristHash;
    friend class Position;
public:
    static constexpr uint8_t DIM {8};
private:
    // The top 18 bitboards to work
    // m_all pieces, black and white (black pawns, white bishops ...)
    bitboard m_pieces_bitboards[2][6] {0};

    // white and black pieces
    bitboard m_side[2] {0};
    bitboard inverse_side[2] {0};

    // m_all the pieces on the board
    bitboard m_all {0};
    bitboard m_empty {0};

public:
    // using short FEN (Forsyth-Edwards Notation):
    // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
    explicit Pieces(const std::string& short_fen);
    void update_bitboards();
    friend bool operator==(const Pieces& left, const Pieces& right);
    friend std::ostream& operator<<(std::ostream& out, const Pieces& pieces);
};

#endif //CHESSAI_PIECES_HPP
