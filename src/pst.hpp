#ifndef ATENIKA_PST_HPP
#define ATENIKA_PST_HPP

#include "defs.hpp"

class Board;

class PieceTables {
private:
    static int32_t piece_sq_tables[COLOR_SIZE][PIECE_SIZE][64];
//    int32_t m_scores[COLOR_SIZE];

public:
    static void init();
    static int32_t get_score(const Board &board);
//    void set_scores(const Board &board);
//
//
//    void add(Color color, PieceType piece, uint8_t sq);
//    void remove(Color color, PieceType piece, uint8_t sq);
//    [[nodiscard]] int32_t get(Color color, GamePhase phase) const;
};

#endif //ATENIKA_PST_HPP
