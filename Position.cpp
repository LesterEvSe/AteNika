#include "Position.hpp"

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