#include "movegen.hpp"

const Board *Movegen::hidden::_board;
MovePicker *Movegen::hidden::_move_picker;

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

void Movegen::hidden::_gen_white_pawn_moves() {
    bitboard move = (_board->get_pieces(WHITE, PAWN) << 8) & _board->get_free_cells();
    bitboard rank8 = move & RANK_8;
    move &= ~RANK_8;
    bitboard long_move = ((move & RANK_3) << 8) & _board->get_free_cells();

    while (move) {
        uint8_t cell = pop_lsb(move);
        _move_picker->emplace_back(Move(cell - 8, cell, PAWN));
    }

    while (rank8) {
        uint8_t cell = pop_lsb(rank8);
        _gen_pawn_promotion(cell - 8, cell);
    }

    while (long_move) {
        uint8_t cell = pop_lsb(long_move);
        _move_picker->emplace_back(Move(cell - 16, cell, PAWN, Move::LONG_PAWN_MOVE));
    }
}
void Movegen::hidden::_gen_black_pawn_moves() {
    bitboard move = (_board->get_pieces(BLACK, PAWN) >> 8) & _board->get_free_cells();
    bitboard rank1 = move & RANK_1; // Promotion rank
    move &= ~RANK_1; // Non-promotion rank

    // If we can make long move (move & RANK_6)
    bitboard long_move = ((move & RANK_6) >> 8) & _board->get_free_cells();

    while (move) {
        uint8_t cell = pop_lsb(move);
        _move_picker->emplace_back(Move(cell + 8, cell, PAWN));
    }

    while (rank1) {
        uint8_t cell = pop_lsb(rank1);
        _gen_pawn_promotion(cell + 8, cell);
    }

    while (long_move) {
        uint8_t cell = pop_lsb(long_move);
        _move_picker->emplace_back(Move(cell + 16, cell, PAWN, Move::LONG_PAWN_MOVE));
    }
}

void Movegen::hidden::_gen_white_left_pawn_captures() {
    // ~FILE_H, because of 16 << 7 = 23. 16 is file A, 23 is file H, it is impossible move
    bitboard attacks = (_board->get_pieces(WHITE, PAWN) << 7) & ~FILE_H;
    uint8_t en_passant_cell = _board->get_en_passant();

    // En passant exists only on the RANK 6 (for white),
    // so if there is 0, it is definitely not en passant!
    // if en passant in cell 40, then we can take pawn on cell 32 from cell 33. So 40 - 7 = 33
    if (en_passant_cell)
        _move_picker->emplace_back(Move(en_passant_cell - 7, en_passant_cell, PAWN, Move::EN_PASSANT, PAWN));

    // If we can take some opponent pieces
    attacks &= _board->get_side_pieces(BLACK);
    bitboard rank8 = attacks & RANK_8;
    attacks &= ~RANK_8;

    while (attacks) {
        uint8_t cell = pop_lsb(attacks);
        PieceType piece = _board->get_piece_at(BLACK, cell);
        _move_picker->emplace_back(Move(cell - 7, cell, PAWN, Move::CAPTURE, piece));
    }

    while (rank8) {
        uint8_t cell = pop_lsb(rank8);
        PieceType piece = _board->get_piece_at(BLACK, cell);
        _gen_pawn_promotion(cell - 7, cell, Move::CAPTURE, piece);
    }
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
