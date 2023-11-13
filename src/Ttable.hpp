#ifndef ATENICA_TTABLE_HPP
#define ATENICA_TTABLE_HPP

#include "defs.hpp"
#include "move.hpp"

class TTEntry {
private:
    Move m_best_move;
    int32_t m_score;
    int16_t m_depth;
    TTFlag m_flag;

public:
    // required for hash table
    TTEntry() = default;
    TTEntry(const Move &move, int32_t score, int16_t depth, TTFlag flag) :
            m_best_move(move), m_score(score), m_depth(depth), m_flag(flag) {}

    [[nodiscard]] Move get_best_move() const;
    [[nodiscard]] int32_t get_score() const;
    [[nodiscard]] int16_t get_depth() const;
    [[nodiscard]] TTFlag get_flag() const;
};


#include "uint96.hpp"
#include "zobrist_hash.hpp"
#include <unordered_map>

namespace Transposition {
namespace hidden {
    // tt - transposition table. Memoization of chess position
    extern std::unordered_map<bits96, TTEntry> _tt;

} // hidden

    bool in_table(const ZobristHash &hash);
    TTEntry get(const ZobristHash &hash);
    void set(const ZobristHash &hash, const TTEntry &entry);
}

#endif //ATENICA_TTABLE_HPP
