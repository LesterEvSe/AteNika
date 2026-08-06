#ifndef ATENIKA_TTABLE_HPP
#define ATENIKA_TTABLE_HPP

#include "uint96.hpp"
#include "zobrist_hash.hpp"
#include "move.hpp"
#include <unordered_map>

struct TTEntry {
    Move move;
    int32_t score;
    int16_t depth;
    TTFlag flag;
};

namespace TTable {
    void add(const ZobristHash &zob, TTEntry entry);
    bool in_table(const ZobristHash &zob);
    const TTEntry &get(const ZobristHash &zob);

    // The table currently outlives every search and every game. bench clears it
    // between positions so a run depends only on the binary, not on whatever was
    // searched before it. Also what UCI "ucinewgame" will need in Phase 0C.
    void clear();
} // namespace TTable

namespace TTable::detail {
    extern std::unordered_map<bits96, TTEntry> _ttable;

} // namespace TTable::detail


#endif // ATENIKA_TTABLE_HPP
