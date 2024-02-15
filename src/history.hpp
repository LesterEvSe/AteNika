#ifndef ATENIKA_HISTORY_HPP
#define ATENIKA_HISTORY_HPP

#include "move.hpp"
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
    void add_and_inc(const ZobristHash &zob_hash, uint8_t ply, uint8_t ep, uint8_t castling_rights);
    const HistoryNode &get_and_dec();
    bool threefold_rule(const Board &board);
    void clear();
} // History

/*
class History {
private:
    // We can record moves until there is no tie for the 50-move rule
    bits96 m_history[MAX_PLY];
    uint8_t m_size;

public:
    History() : m_size(0) {}
    void operator=(const History &history);

    void clear();
    bool add_pos(const ZobristHash &zob_hash);
    [[nodiscard]] bool threefold_repetition(const ZobristHash &zob_hash) const;
};
*/

#endif //ATENIKA_HISTORY_HPP
