#ifndef ATENIKA_HISTORY_HPP
#define ATENIKA_HISTORY_HPP

#include "zobrist_hash.hpp"

struct HistoryNode {
    uint96 hash;
    uint8_t ply;
    uint8_t ep; // en_passant_cell
    uint8_t castling_rights;
};

namespace History {
namespace hidden {
    extern int16_t _counter;
    extern HistoryNode _history[MAX_MOVES];

} // hidden
    void init();
    int16_t get_ply();

    // When we momentarily exit the search, then we don't put ply back in place,
    // so it needs to be set to the old position
    void set_ply(int16_t ply);
    void add_and_inc(const ZobristHash &zob_hash, uint8_t ply, uint8_t ep, uint8_t castling_rights);

    const HistoryNode &get_and_dec();
    bool threefold_rule(const Board &board);
    void clear();
} // History

#endif //ATENIKA_HISTORY_HPP
