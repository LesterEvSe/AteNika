#ifndef ATENICA_MOVEGEN_HPP
#define ATENICA_MOVEGEN_HPP

#include "board.hpp"
#include "movepicker.hpp"

namespace Movegen {
    namespace hidden {
        // helper private data, to avoid throwing them over function arguments
        extern const Board *_board;
        extern MovePicker *_move_picker;

        void _gen_white_moves();
        void _gen_black_moves();

        void _gen_white_pawn_moves_and_captures();
        void _gen_black_pawn_moves_and_captures();

        void _gen_pawn_promotion(uint8_t from, uint8_t to, Move::Flag flag = Move::QUIET, PieceType captured_piece = NONE);
        void _gen_white_pawn_moves();
        void _gen_black_pawn_moves();

        void _gen_white_left_pawn_captures();
        void _gen_black_left_pawn_captures();
        void _gen_white_right_pawn_captures();
        void _gen_black_right_pawn_captures();

        void _gen_white_king_moves();
        void _gen_black_king_moves();

        void _gen_white_knight_moves();
        void _gen_black_knight_moves();

        void _gen_white_rook_moves();
        void _gen_black_rook_moves();

        void _gen_white_bishop_moves();
        void _gen_black_bishop_moves();

        void _gen_white_queen_moves();
        void _gen_black_queen_moves();
    }
    void gen_moves(const Board *board, MovePicker *move_picker);
}

#endif //ATENICA_MOVEGEN_HPP
