#ifndef CHESSAI_BOARD_HPP
#define CHESSAI_BOARD_HPP

#include "defs.hpp"
#include "bitfunc.hpp"
#include <string>

class Board {
private:

    // The top 14 bitboards to work
    // m_all, black and white pieces (black pawns, white bishops ...)
    bitboard m_pieces[2][6] {0};

    // white and black, all pieces
    bitboard m_side[2] {0};
    bitboard m_all {0};

    Color m_player_move = WHITE;

    // The moves that can be brought back.
    // If > 50 moves, there will be a draw
    uint8_t m_half_move_counter;

    // 4 bits
    // 0 - white queenside
    // 1 - white kingside
    // 2 - black queenside
    // 3 - black kingside
    uint8_t m_castling_rights;


public:
    // using short FEN (Forsyth-Edwards Notation):
    // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
    Board(std::string short_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    void update_bitboards();

    Color get_curr_player_move();
    Color get_opponent_player_move();

    friend std::ostream& operator<<(std::ostream& out, const Board& board);
};


#endif //CHESSAI_BOARD_HPP
