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

void Movegen::hidden::_gen_white_pawn_moves() {

}
void Movegen::hidden::_gen_black_pawn_moves() {

}

void Movegen::hidden::_gen_white_left_pawn_captures() {

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
