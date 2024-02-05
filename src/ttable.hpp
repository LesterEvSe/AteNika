#ifndef ATENIKA_TTABLE_HPP
#define ATENIKA_TTABLE_HPP

#include "uint96.hpp"
#include "zobrist_hash.hpp"
#include "move.hpp"

#include <unordered_map>

namespace Ttable {
namespace hidden {
    extern std::unordered_map<bits96, Move> _ttable;

} // hidden
    void add(const ZobristHash &zob, Move move);
    Move get(const ZobristHash &zob);
} // Ttable


#endif //ATENIKA_TTABLE_HPP
