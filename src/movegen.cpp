#include "movegen.hpp"

Movegen::Movegen(const Board &board) : m_board(board) {
    gen_moves();
}

MoveList &Movegen::get_moves() {
    return m_moves;
}

MoveList &Movegen::get_legal_moves() {
    if (m_legal_moves.size() == 0)
        gen_legal_moves();
    return m_legal_moves;
}

void Movegen::add_moves(uint8_t from, bitboard moves, PieceType piece) {
    while (moves) {
        uint8_t cell = pop_lsb(moves);
        m_moves.emplace_back(Move(from, cell, piece));
    }
}
void Movegen::add_attacks(uint8_t from, bitboard attacks, PieceType move_piece, Color defender) {
    while (attacks) {
        uint8_t cell = pop_lsb(attacks);
        PieceType piece = m_board.get_piece_at(defender, cell);
        m_moves.emplace_back(Move(from, cell, move_piece, Move::CAPTURE, piece));
    }
}

void Movegen::gen_moves() {
    if (m_board.get_curr_move() == WHITE)
        gen_white_moves();
    else
        gen_black_moves();
}

void Movegen::gen_legal_moves() {
    Color curr_player = m_board.get_curr_move();
    for (uint8_t i = 0; i < m_moves.size(); ++i) {
        Board temp = m_board;
        temp.make(m_moves[i]);

        if (!temp.king_in_check(curr_player))
            m_legal_moves.emplace_back(m_moves[i]);
    }
}

void Movegen::gen_white_moves() {
    gen_white_pawn_moves_and_captures();
    gen_white_king_moves();
    gen_white_knight_moves();
    gen_white_rook_moves();
    gen_white_bishop_moves();
    gen_white_queen_moves();
}
void Movegen::gen_black_moves() {
    gen_black_pawn_moves_and_captures();
    gen_black_king_moves();
    gen_black_knight_moves();
    gen_black_rook_moves();
    gen_black_bishop_moves();
    gen_black_queen_moves();
}

void Movegen::gen_white_pawn_moves_and_captures() {
    gen_white_pawn_moves();
    gen_white_left_pawn_captures();
    gen_white_right_pawn_captures();
}
void Movegen::gen_black_pawn_moves_and_captures() {
    gen_black_pawn_moves();
    gen_black_left_pawn_captures();
    gen_black_right_pawn_captures();
}

// If the move is QUIET, then it is not a CAPTURE.
void Movegen::gen_pawn_promotion(uint8_t from, uint8_t to, Move::Flag flag, PieceType captured_piece) {
    flag = Move::Flag(Move::PROMOTION | flag);
    m_moves.emplace_back(Move(from, to, PAWN, flag, captured_piece, QUEEN));
    m_moves.emplace_back(Move(from, to, PAWN, flag, captured_piece, KNIGHT));
    m_moves.emplace_back(Move(from, to, PAWN, flag, captured_piece, BISHOP));
    m_moves.emplace_back(Move(from, to, PAWN, flag, captured_piece, ROOK));
}

void Movegen::gen_white_pawn_moves() {
    bitboard move = (m_board.get_pieces(WHITE, PAWN) << 8) & m_board.get_free_cells();
    bitboard rank8 = move & RANK_8;
    move &= ~RANK_8;
    bitboard long_move = ((move & RANK_3) << 8) & m_board.get_free_cells();

    while (move) {
        uint8_t cell = pop_lsb(move);
        m_moves.emplace_back(Move(cell - 8, cell, PAWN));
    }

    while (rank8) {
        uint8_t cell = pop_lsb(rank8);
        gen_pawn_promotion(cell - 8, cell);
    }

    while (long_move) {
        uint8_t cell = pop_lsb(long_move);
        m_moves.emplace_back(Move(cell - 16, cell, PAWN, Move::LONG_PAWN_MOVE));
    }
}
void Movegen::gen_black_pawn_moves() {
    bitboard move = (m_board.get_pieces(BLACK, PAWN) >> 8) & m_board.get_free_cells();
    bitboard rank1 = move & RANK_1; // Promotion rank
    move &= ~RANK_1; // Non-promotion rank

    // If we can make long move (move & RANK_6)
    bitboard long_move = ((move & RANK_6) >> 8) & m_board.get_free_cells();

    while (move) {
        uint8_t cell = pop_lsb(move);
        m_moves.emplace_back(Move(cell + 8, cell, PAWN));
    }

    while (rank1) {
        uint8_t cell = pop_lsb(rank1);
        gen_pawn_promotion(cell + 8, cell);
    }

    while (long_move) {
        uint8_t cell = pop_lsb(long_move);
        m_moves.emplace_back(Move(cell + 16, cell, PAWN, Move::LONG_PAWN_MOVE));
    }
}

void Movegen::gen_white_left_pawn_captures() {
    // ~FILE_H, because of 16 << 7 = 23. 16 is file A, 23 is file H, it is impossible move
    bitboard attacks = (m_board.get_pieces(WHITE, PAWN) << 7) & ~FILE_H;
    uint8_t en_passant_cell = m_board.get_en_passant();

    // Checking the cel in advance. If en_passant does not exist (0)
    // there will be no further check due to lazy calculations &&
    // if en passant in cell 40, then we can take pawn on cell 32 from cell 33. So 40 - 7 = 33
    if (en_passant_cell && (ONE << en_passant_cell) & attacks)
        m_moves.emplace_back(Move(en_passant_cell - 7, en_passant_cell, PAWN, Move::EN_PASSANT, PAWN));

    // If we can take some opponent pieces
    attacks &= m_board.get_side_pieces(BLACK);
    bitboard rank8 = attacks & RANK_8;
    attacks &= ~RANK_8;

    while (attacks) {
        uint8_t cell = pop_lsb(attacks);
        PieceType piece = m_board.get_piece_at(BLACK, cell);
        m_moves.emplace_back(Move(cell - 7, cell, PAWN, Move::CAPTURE, piece));
    }

    while (rank8) {
        uint8_t cell = pop_lsb(rank8);
        PieceType piece = m_board.get_piece_at(BLACK, cell);
        gen_pawn_promotion(cell - 7, cell, Move::CAPTURE, piece);
    }
}
void Movegen::gen_black_left_pawn_captures() {
    bitboard attacks = (m_board.get_pieces(BLACK, PAWN) >> 9) & ~FILE_H;
    uint8_t en_passant_cell = m_board.get_en_passant();

    if (en_passant_cell && (ONE << en_passant_cell) & attacks)
        m_moves.emplace_back(Move(en_passant_cell + 9, en_passant_cell, PAWN, Move::EN_PASSANT, PAWN));

    attacks &= m_board.get_side_pieces(WHITE);
    bitboard rank1 = attacks & RANK_1;
    attacks &= ~RANK_1;

    while (attacks) {
        uint8_t cell = pop_lsb(attacks);
        PieceType piece = m_board.get_piece_at(WHITE, cell);
        m_moves.emplace_back(Move(cell + 9, cell, PAWN, Move::CAPTURE, piece));
    }

    while (rank1) {
        uint8_t cell = pop_lsb(rank1);
        PieceType piece = m_board.get_piece_at(WHITE, cell);
        gen_pawn_promotion(cell + 9, cell, Move::CAPTURE, piece);
    }
}
void Movegen::gen_white_right_pawn_captures() {
    bitboard attacks = (m_board.get_pieces(WHITE, PAWN) << 9) & ~FILE_A;
    uint8_t en_passant_cell = m_board.get_en_passant();

    if (en_passant_cell && (ONE << en_passant_cell) & attacks)
        m_moves.emplace_back(Move(en_passant_cell - 9, en_passant_cell, PAWN, Move::EN_PASSANT, PAWN));

    attacks &= m_board.get_side_pieces(BLACK);
    bitboard rank8 = attacks & RANK_8;
    attacks &= ~RANK_8;

    while (attacks) {
        uint8_t cell = pop_lsb(attacks);
        PieceType piece = m_board.get_piece_at(BLACK, cell);
        m_moves.emplace_back(Move(cell - 9, cell, PAWN, Move::CAPTURE, piece));
    }

    while (rank8) {
        uint8_t cell = pop_lsb(rank8);
        PieceType piece = m_board.get_piece_at(BLACK, cell);
        gen_pawn_promotion(cell - 9, cell, Move::CAPTURE, piece);
    }
}
void Movegen::gen_black_right_pawn_captures() {
    bitboard attacks = (m_board.get_pieces(BLACK, PAWN) >> 7) & ~FILE_A;
    uint8_t en_passant_cell = m_board.get_en_passant();

    if (en_passant_cell && (ONE << en_passant_cell) & attacks)
        m_moves.emplace_back(Move(en_passant_cell + 7, en_passant_cell, PAWN, Move::EN_PASSANT, PAWN));

    attacks &= m_board.get_side_pieces(WHITE);
    bitboard rank1 = attacks & RANK_1;
    attacks &= ~RANK_1;

    while (attacks) {
        uint8_t cell = pop_lsb(attacks);
        PieceType piece = m_board.get_piece_at(WHITE, cell);
        m_moves.emplace_back(Move(cell + 7, cell, PAWN, Move::CAPTURE, piece));
    }

    while (rank1) {
        uint8_t cell = pop_lsb(rank1);
        PieceType piece = m_board.get_piece_at(WHITE, cell);
        gen_pawn_promotion(cell + 7, cell, Move::CAPTURE, piece);
    }
}

void Movegen::gen_white_king_moves() {
    uint8_t king_cell = lsb(m_board.get_pieces(WHITE, KING));
    if (m_board.can_white_ks_castle())
        m_moves.emplace_back(Move(king_cell, king_cell + 2, KING, Move::KSIDE_CASTLING));
    if (m_board.can_white_qs_castle())
        m_moves.emplace_back(Move(king_cell, king_cell - 2, KING, Move::QSIDE_CASTLING));

    bitboard moves = Attacks::get_king_attacks(king_cell);
    bitboard attacks = moves & m_board.get_side_pieces(BLACK);
    moves &= m_board.get_free_cells();

    add_moves(king_cell, moves, KING);
    add_attacks(king_cell, attacks, KING, BLACK);
}
void Movegen::gen_black_king_moves() {
    uint8_t king_cell = lsb(m_board.get_pieces(BLACK, KING));
    if (m_board.can_black_ks_castle())
        m_moves.emplace_back(Move(king_cell, king_cell + 2, KING, Move::KSIDE_CASTLING));
    if (m_board.can_black_qs_castle())
        m_moves.emplace_back(Move(king_cell, king_cell - 2, KING, Move::QSIDE_CASTLING));

    bitboard moves = Attacks::get_king_attacks(king_cell);
    bitboard attacks = moves & m_board.get_side_pieces(WHITE);
    moves &= m_board.get_free_cells();

    add_moves(king_cell, moves, KING);
    add_attacks(king_cell, attacks, KING, WHITE);
}

void Movegen::gen_white_knight_moves() {
    bitboard knights = m_board.get_pieces(WHITE, KNIGHT);
    bitboard free_cells = m_board.get_free_cells();

    while (knights) {
        uint8_t cell = pop_lsb(knights);

        bitboard moves = Attacks::get_knight_attacks(cell);
        bitboard attacks = moves & m_board.get_side_pieces(BLACK);
        moves &= free_cells;

        add_moves(cell, moves, KNIGHT);
        add_attacks(cell, attacks, KNIGHT, BLACK);
    }
}
void Movegen::gen_black_knight_moves() {
    bitboard knights = m_board.get_pieces(BLACK, KNIGHT);
    bitboard free_cells = m_board.get_free_cells();

    while (knights) {
        uint8_t cell = pop_lsb(knights);

        bitboard moves = Attacks::get_knight_attacks(cell);
        bitboard attacks = moves & m_board.get_side_pieces(WHITE);
        moves &= free_cells;

        add_moves(cell, moves, KNIGHT);
        add_attacks(cell, attacks, KNIGHT, WHITE);
    }
}

void Movegen::gen_white_rook_moves() {
    bitboard rooks = m_board.get_pieces(WHITE, ROOK);
    bitboard free_cells = m_board.get_free_cells();

    while (rooks) {
        uint8_t cell = pop_lsb(rooks);

        bitboard moves = Attacks::get_rook_attacks(cell,  m_board.get_all_pieces());
        bitboard attacks = moves & m_board.get_side_pieces(BLACK);
        moves &= free_cells;

        add_moves(cell, moves, ROOK);
        add_attacks(cell, attacks, ROOK, BLACK);
    }
}
void Movegen::gen_black_rook_moves() {
    bitboard rooks = m_board.get_pieces(BLACK, ROOK);
    bitboard free_cells = m_board.get_free_cells();

    while (rooks) {
        uint8_t cell = pop_lsb(rooks);

        bitboard moves = Attacks::get_rook_attacks(cell,  m_board.get_all_pieces());
        bitboard attacks = moves & m_board.get_side_pieces(WHITE);
        moves &= free_cells;

        add_moves(cell, moves, ROOK);
        add_attacks(cell, attacks, ROOK, WHITE);
    }
}

void Movegen::gen_white_bishop_moves() {
    bitboard bishops = m_board.get_pieces(WHITE, BISHOP);
    bitboard free_cells = m_board.get_free_cells();

    while (bishops) {
        uint8_t cell = pop_lsb(bishops);

        bitboard moves = Attacks::get_bishop_attacks(cell,  m_board.get_all_pieces());
        bitboard attacks = moves & m_board.get_side_pieces(BLACK);
        moves &= free_cells;

        add_moves(cell, moves, BISHOP);
        add_attacks(cell, attacks, BISHOP, BLACK);
    }

}
void Movegen::gen_black_bishop_moves() {
    bitboard bishops = m_board.get_pieces(BLACK, BISHOP);
    bitboard free_cells = m_board.get_free_cells();

    while (bishops) {
        uint8_t cell = pop_lsb(bishops);

        bitboard moves = Attacks::get_bishop_attacks(cell,  m_board.get_all_pieces());
        bitboard attacks = moves & m_board.get_side_pieces(WHITE);
        moves &= free_cells;

        add_moves(cell, moves, BISHOP);
        add_attacks(cell, attacks, BISHOP, WHITE);
    }
}

void Movegen::gen_white_queen_moves() {
    bitboard queens = m_board.get_pieces(WHITE, QUEEN);
    bitboard free_cells = m_board.get_free_cells();

    while (queens) {
        uint8_t cell = pop_lsb(queens);

        bitboard moves = Attacks::get_queen_attacks(cell,  m_board.get_all_pieces());
        bitboard attacks = moves & m_board.get_side_pieces(BLACK);
        moves &= free_cells;

        add_moves(cell, moves, QUEEN);
        add_attacks(cell, attacks, QUEEN, BLACK);
    }
}
void Movegen::gen_black_queen_moves() {
    bitboard queens = m_board.get_pieces(BLACK, QUEEN);
    bitboard free_cells = m_board.get_free_cells();

    while (queens) {
        uint8_t cell = pop_lsb(queens);

        bitboard moves = Attacks::get_queen_attacks(cell, m_board.get_all_pieces());
        bitboard attacks = moves & m_board.get_side_pieces(WHITE);
        moves &= free_cells;

        add_moves(cell, moves, QUEEN);
        add_attacks(cell, attacks, QUEEN, WHITE);
    }
}
