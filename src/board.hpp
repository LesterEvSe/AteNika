#ifndef ATENICA_BOARD_HPP
#define ATENICA_BOARD_HPP

#include "defs.hpp"
#include "bitfunc.hpp"
#include "attacks.hpp"
#include "zobrist_hash.hpp"
#include "move.hpp"

class Board {
private:
    // enums COLOR_SIZE and PIECE_SIZE in defs.hpp
    // The top 15 bitboards to work
    // m_all, black and white pieces (black pawns, white bishops ...)
    bitboard m_pieces[COLOR_SIZE][PIECE_SIZE] {0};

    // white and black, all pieces
    bitboard m_side[COLOR_SIZE] {0};
    bitboard m_all {0};

    Color m_player_move;

    // ply is half move
    // The moves that can be brought back.
    // If >= 50 moves, there will be a draw
    uint8_t m_ply;

    uint8_t m_en_passant_cell;

    // if white rook move, example 7 cell, we lose white kingside CASTLING.
    // So, m_castling_rights & CASTLING[7] = 1111 & 1101 = 1101
    static constexpr uint8_t CASTLING[64] = {
            13, 15, 15, 15, 12, 15, 15, 14,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            7, 15, 15, 15, 3,  15, 15, 11
    };

    // Using the Fen order
    //
    // 4 bits
    // 0001 - white kingside
    // 0010 - white queenside
    // 0100 - black kingside
    // 1000 - black queenside
    uint8_t m_castling_rights {0};

    ZobristHash m_hash;

    void add_piece(Color color, PieceType piece, uint8_t cell);
    void remove_piece(Color color, PieceType piece, uint8_t cell);

public:
    // using short FEN (Forsyth-Edwards Notation). detailed in defs.hpp
    // 1. Pieces
    // 2. White or black move
    // 3. Castling rights
    // 4. En passant cell
    // 5. ply (half moves)
    explicit Board(std::string short_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0");
    void update_bitboards();

    [[nodiscard]] Color get_curr_move() const;
    [[nodiscard]] Color get_opponent_move() const;

    [[nodiscard]] bitboard get_pieces(Color color, PieceType type) const;
    [[nodiscard]] bitboard get_side_pieces(Color color) const;
    [[nodiscard]] bitboard get_all_pieces() const;
    [[nodiscard]] bitboard get_free_cells() const;

    [[nodiscard]] uint8_t get_ply() const;
    [[nodiscard]] uint8_t get_en_passant() const;

    // ks - king side
    // qs - queen side
    [[nodiscard]] bool get_white_ks_castle() const;
    [[nodiscard]] bool get_white_qs_castle() const;
    [[nodiscard]] bool get_black_ks_castle() const;
    [[nodiscard]] bool get_black_qs_castle() const;

    [[nodiscard]] bool can_white_ks_castle() const;
    [[nodiscard]] bool can_white_qs_castle() const;
    [[nodiscard]] bool can_black_ks_castle() const;
    [[nodiscard]] bool can_black_qs_castle() const;

    [[nodiscard]] PieceType get_piece_at(Color color, uint8_t index) const;
    [[nodiscard]] bool king_in_check(Color color) const;
    [[nodiscard]] bool under_attack(Color defender, uint8_t cell) const;

    void make(const Move &move);

    friend std::ostream& operator<<(std::ostream &out, const Board &board);
};

#endif //ATENICA_BOARD_HPP
