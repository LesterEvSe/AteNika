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

namespace Ttable {
namespace hidden {
    extern std::unordered_map<bits96, TTEntry> _ttable;

} // hidden
    void add(const ZobristHash &zob, TTEntry entry);
    bool in_table(const ZobristHash &zob);
    const TTEntry &get(const ZobristHash &zob);
} // Ttable


#endif //ATENIKA_TTABLE_HPP
