#ifndef CHESSAI_POSITION_HPP
#define CHESSAI_POSITION_HPP

#include "pieces.hpp"
#include "ZobristHash.hpp"
#include "PosesHistory.hpp"

class Position {
private:
    Pieces m_pieces;
    uint8_t m_en_passant;

    // Castling rights
    bool m_ws_castling;
    bool m_wl_castling;
    bool m_bs_castling;
    bool m_bl_castling;

    bool m_w_castling_true {false};
    bool m_b_castling_true {false};

    bool m_black_move;
    ZobristHash m_hash;
    uint8_t m_50moves_coun {0};
    PosesHistory m_pos_history;

public:
    Position(const std::string& short_fen, uint8_t en_passant, bool ws_cast,
             bool wl_cast, bool bs_cast, bool bl_cast, bool black_move);

    void add_piece(uint8_t square, uint8_t side, uint8_t type);
    void remove_piece(uint8_t squre, uint8_t side, uint8_t type);
    void change_en_passant(uint8_t en_passant);

    void remove_ws_castling();
    void remove_wl_castling();
    void remove_bs_castling();
    void remove_bl_castling();

    void update_move_coun();
    void update_50moves_coun(bool reset);
};


#endif //CHESSAI_POSITION_HPP
