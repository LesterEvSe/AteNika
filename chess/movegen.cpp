#include "movegen.hpp"

void Movegen::gen_moves(const Board *board, MovePicker *move_picker) {
    hidden::_board = board;
    hidden::_move_picker = move_picker;

    if (hidden::_board->get_curr_player_move() == WHITE)
        hidden::_gen_white_moves();
    else
        hidden::_gen_black_moves();

    // To avoid problems with changing data that should not be changed
    hidden::_board = nullptr;
    hidden::_move_picker = nullptr;
}

void Movegen::hidden::_gen_white_moves() {
    _gen_white_pawn_moves_and_captures();
    _gen_white_king_moves();
    _gen_white_knight_moves();
    _gen_white_rook_moves();
    _gen_white_bishop_moves();
    _gen_white_queen_moves();
}
void Movegen::hidden::_gen_black_moves() {
    _gen_black_pawn_moves_and_captures();
    _gen_black_king_moves();
    _gen_black_knight_moves();
    _gen_black_rook_moves();
    _gen_black_bishop_moves();
    _gen_black_queen_moves();
}

void Movegen::hidden::_gen_white_pawn_moves_and_captures() {
    _gen_white_pawn_moves();
    _gen_white_left_pawn_captures();
    _gen_white_right_pawn_captures();
}
void Movegen::hidden::_gen_black_pawn_moves_and_captures() {
    _gen_black_pawn_moves();
    _gen_black_left_pawn_captures();
    _gen_black_right_pawn_captures();
}

// If the move is QUIET, then it is not a CAPTURE.
void Movegen::hidden::_gen_pawn_promotion(uint8_t from, uint8_t to, Move::Flag flag, PieceType captured_piece) {
    flag = Move::Flag(Move::PROMOTION | flag);
    _move_picker->emplace_back(Move(from, to, PAWN, flag, captured_piece, QUEEN));
    _move_picker->emplace_back(Move(from, to, PAWN, flag, captured_piece, KNIGHT));
    _move_picker->emplace_back(Move(from, to, PAWN, flag, captured_piece, BISHOP));
    _move_picker->emplace_back(Move(from, to, PAWN, flag, captured_piece, ROOK));
}

// TODO. We can divide moves to RANK_8 and other to get rid of the "if" block
void Movegen::hidden::_gen_white_pawn_moves() {
    bitboard move = (_board->get_pieces(WHITE, PAWN) << 8) & _board->get_free_cells();
    bitboard long_move = (move << 8) & _board->get_free_cells();

    while (move) {
        uint8_t cell = pop_lsb(move);

        // TODO. Rewrite if block
        if (cell < 56)
            _move_picker->emplace_back(Move(cell - 8, cell, PAWN));
        else // If RANK_8 for white
            _gen_pawn_promotion(cell - 8, cell);
    }

    while(long_move) {
        uint8_t cell = pop_lsb(long_move);
        _move_picker->emplace_back(Move(cell - 16, cell, PAWN, Move::LONG_PAWN_MOVE));
    }
}
void Movegen::hidden::_gen_black_pawn_moves() {
    bitboard move = (_board->get_pieces(BLACK, PAWN) >> 8) & _board->get_free_cells();
    bitboard long_move = (move >> 8) & _board->get_free_cells();

    while (move) {
        uint8_t cell = pop_lsb(move);

        // TODO. Rewrite if block
        if (cell > 7)
            _move_picker->emplace_back(Move(cell + 8, cell, PAWN));
        else // If RANK_1 for black
            _gen_pawn_promotion(cell + 8, cell);
    }

    while (long_move) {
        uint8_t cell = pop_lsb(long_move);
        _move_picker->emplace_back(Move(cell + 16, cell, PAWN, Move::LONG_PAWN_MOVE));
    }
}

/**
 *      A   B   C   D   E   F   G   H
 * 8 | 56  57  58  59  60  61  62  63
 * 7 | 48  49  50  51  52  53  54  55
 * 6 | 40  41  42  43  44  45  46  47
 * 5 | 32  33  34  35  36  37  38  39
 * 4 | 24  25  26  27  28  29  30  31
 * 3 | 16  17  18  19  20  21  22  23
 * 2 |  8   9  10  11  12  13  14  15
 * 1 |  0   1   2   3   4   5   6   7
 */

void Movegen::hidden::_gen_white_left_pawn_captures() {
    bitboard attacks = (_board->get_pieces(WHITE, PAWN) << 7) & ~FILE_H;
    uint8_t en_passant_cell = _board->get_en_passant();

    // En passant exists only on the RANK 6 (for white),
    // so if there is 0, it is definitely not en passant!
    // if en passant in cell 40, then we can take pawn on cell 32 from cell 33. So 40 - 7 = 33
    if (en_passant_cell)
        _move_picker->emplace_back(Move(en_passant_cell-7, en_passant_cell, PAWN, Move::EN_PASSANT, PAWN));

    // If we can take some opponent pieces
    attacks &= _board->get_side_pieces(BLACK);

    // TODO. Need to determine capture and promotion
}
void Movegen::hidden::_gen_black_left_pawn_captures() {

}
void Movegen::hidden::_gen_white_right_pawn_captures() {

}
void Movegen::hidden::_gen_black_right_pawn_captures() {

}

void Movegen::hidden::_gen_white_king_moves() {

}
void Movegen::hidden::_gen_black_king_moves() {

}

void Movegen::hidden::_gen_white_knight_moves() {

}
void Movegen::hidden::_gen_black_knight_moves() {

}

void Movegen::hidden::_gen_white_rook_moves() {

}
void Movegen::hidden::_gen_black_rook_moves() {

}

void Movegen::hidden::_gen_white_bishop_moves() {

}
void Movegen::hidden::_gen_black_bishop_moves() {

}

void Movegen::hidden::_gen_white_queen_moves() {

}
void Movegen::hidden::_gen_black_queen_moves() {

}
