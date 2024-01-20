#include "Ttable.hpp"

Move TTEntry::get_best_move() const { return m_best_move; }
int32_t TTEntry::get_score()  const { return m_score; }
int16_t TTEntry::get_depth()  const { return m_depth; }
TTFlag TTEntry::get_flag()    const { return m_flag; }


std::unordered_map<bits96, TTEntry> Transposition::hidden::_tt;

bool Transposition::in_table(const ZobristHash &zob_hash) {
    return hidden::_tt.find((bits96)zob_hash.get_hash()) != hidden::_tt.end();
}

void Transposition::clear() {
    hidden::_tt.clear();
}

TTEntry Transposition::get(const ZobristHash &zob_hash) {
    return hidden::_tt[(bits96)zob_hash.get_hash()];
}

void Transposition::set(const ZobristHash &zob_hash, const TTEntry &entry) {
    hidden::_tt[(bits96)zob_hash.get_hash()] = entry;
}