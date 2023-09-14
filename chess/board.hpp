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
    uint8_t m_half_moves_counter;

    uint64_t m_en_passant;

    // if white rook move, example 7 cell, we lose white kingside castling.
    // So, m_castling_rights & castling[7] = 1111 & 1101 = 1101
    static constexpr uint64_t castling[64] = {
            14, 15, 15, 15, 12, 15, 15, 13,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            11, 15, 15, 15, 3, 15, 15, 7
    };

    // 4 bits
    // 1 - white queenside
    // 2 - white kingside
    // 4 - black queenside
    // 8 - black kingside
    uint8_t m_castling_rights;


public:
    // using short FEN (Forsyth-Edwards Notation):
    // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
    explicit Board(std::string short_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    void update_bitboards();

    [[nodiscard]] Color get_curr_player_move() const;
    [[nodiscard]] Color get_opponent_player_move() const;

    [[nodiscard]] bitboard get_pieces(Color color, PieceType piece) const;
    [[nodiscard]] bitboard get_side_pieces(Color color) const;
    [[nodiscard]] bitboard get_all_pieces() const;
    [[nodiscard]] bitboard get_free_cells() const;

    [[nodiscard]] uint8_t get_half_moves() const;
    [[nodiscard]] uint8_t get_en_passant() const;

    [[nodiscard]] bool get_white_qs_castle() const;
    [[nodiscard]] bool get_white_ks_castle() const;
    [[nodiscard]] bool get_black_qs_castle() const;
    [[nodiscard]] bool get_black_ks_castle() const;

    // bool color_is_in_check?
    // bool cell_under_attack. This two func maybe same?
    // bitboard get_all_moves (include captures, maybe :) )
    // bitboard get_captures

    friend std::ostream& operator<<(std::ostream& out, const Board& board);
};

#endif //CHESSAI_BOARD_HPP
