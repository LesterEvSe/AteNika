#include "position.hpp"

Position::Position(const std::string& short_fen, uint8_t en_passant, bool ws_cast,
                   bool wl_cast, bool bs_cast, bool bl_cast, bool black_move) :
          m_pieces(short_fen),
          m_en_passant(en_passant),

          m_ws_castling(ws_cast),
          m_wl_castling(wl_cast),
          m_bs_castling(bs_cast),
          m_bl_castling(bl_cast),

          m_black_move(black_move),
          m_hash(m_pieces, black_move, ws_cast,
                 wl_cast, bs_cast, bl_cast)
{
    m_pos_history.add_pos(m_hash);
}

void Position::move(const Move &move)
{
    remove_piece(move.m_from, move.m_attacker_side, move.m_attacker_type);
    add_piece(move.m_to, move.m_attacker_side, move.m_attacker_type);
    if (move.m_defender_type != 255)
        remove_piece(move.m_to, move.m_attacker_side, move.m_attacker_type);

    switch (move.m_flag)
    {
        default:
        case Move::Flag::DEFAULT:
            break;

        case Move::Flag::PAWN_LONG_MOVE:
            change_en_passant((move.m_from + move.m_to) >> 1);
            break;
        case Move::Flag::EN_PASSANT_CAPTURE:
            if (move.m_attacker_side == WHITE) remove_piece(move.m_to - 8, BLACK, PAWN);
            else remove_piece(move.m_to + 8, WHITE, PAWN);
            break;

        case Move::Flag::WS_CASTLING:
            remove_piece(7, WHITE, ROOK);
            add_piece(5, WHITE, ROOK);
            m_w_castling_true = true;
            break;
        case Move::Flag::WL_CASTLING:
            remove_piece(0, WHITE, ROOK);
            add_piece(3, WHITE, ROOK);
            m_w_castling_true = true;
            break;

        case Move::Flag::BS_CASTLING:
            remove_piece(63, BLACK, ROOK);
            add_piece(61, BLACK, ROOK);
            m_b_castling_true = true;
            break;
        case Move::Flag::BL_CASTLING:
            remove_piece(56, BLACK, ROOK);
            add_piece(59, BLACK, ROOK);
            m_b_castling_true = true;
            break;

        case Move::Flag::PROMOTE_TO_QUEEN:
            remove_piece(move.m_from, move.m_attacker_side, PAWN);
            add_piece(move.m_from, move.m_attacker_side, QUEEN);
            break;
        case Move::Flag::PROMOTE_TO_KNIGHT:
            remove_piece(move.m_from, move.m_attacker_side, PAWN);
            add_piece(move.m_from, move.m_attacker_side, KNIGHT);
            break;
        case Move::Flag::PROMOTE_TO_BISHOP:
            remove_piece(move.m_from, move.m_attacker_side, PAWN);
            add_piece(move.m_from, move.m_attacker_side, BISHOP);
            break;
        case Move::Flag::PROMOTE_TO_ROOK:
            remove_piece(move.m_from, move.m_attacker_side, PAWN);
            add_piece(move.m_from, move.m_attacker_side, ROOK);
            break;
    }

    m_pieces.update_bitboards();

    // If not a long pawn move, we can not make a capture on the pass
    if (move.m_flag != Move::Flag::PAWN_LONG_MOVE)
        change_en_passant(255);

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
    switch(move.m_from)
    {
        // WHITE
        case 0:
            remove_wl_castling();
            break;
        case 4:
            remove_ws_castling();
            remove_wl_castling();
            break;
        case 7:
            remove_ws_castling();
            break;

        // BLACK
        case 56:
            remove_bl_castling();
            break;
        case 60:
            remove_bs_castling();
            remove_bl_castling();
            break;
        case 63:
            remove_bs_castling();
        default:
            break;
    }

    update_move_coun();
    if (move.m_attacker_type == PAWN || move.m_defender_type != 255) {
        update_50moves_coun(true);
        m_pos_history.clear();
    }
    update_50moves_coun(false);
    m_pos_history.add_pos(m_hash);
}

void Position::add_piece(uint8_t square, uint8_t side, uint8_t type) {
    if (get_bit(m_pieces.m_pieces_bitboards[side][type], square)) {
        set1(m_pieces.m_pieces_bitboards[side][type], square);
        m_hash.xor_piece(square, side, type);
    }
}
void Position::remove_piece(uint8_t square, uint8_t side, uint8_t type) {
    if (get_bit(m_pieces.m_pieces_bitboards[side][type], square)) {
        set0(m_pieces.m_pieces_bitboards[side][type], square);
        m_hash.xor_piece(square, side, type);
    }
}
void Position::change_en_passant(uint8_t en_passant) { m_en_passant =  en_passant; }

void Position::remove_ws_castling() {
    if (m_ws_castling) {
        m_ws_castling = false;
        m_hash.xor_ws_castling();
    }
}
void Position::remove_wl_castling() {
    if (m_wl_castling) {
        m_wl_castling = false;
        m_hash.xor_bl_castling();
    }
}
void Position::remove_bs_castling() {
    if (m_bs_castling) {
        m_bs_castling = false;
        m_hash.xor_bs_castling();
    }
}
void Position::remove_bl_castling() {
    if (m_bl_castling) {
        m_bl_castling = false;
        m_hash.xor_bl_castling();
    }
}

void Position::update_move_coun() { m_black_move ^= true; }
void Position::update_50moves_coun(bool reset) {
    m_50moves_coun = reset ? 0 : ++m_50moves_coun;
}
