#include "search/ttable.hpp"

std::unordered_map<bits96, TTEntry> TTable::detail::_ttable;

void TTable::add(const ZobristHash &zob, TTEntry entry) {
    detail::_ttable[(bits96) zob.get_hash()] = entry;
}

bool TTable::in_table(const ZobristHash &zob) {
    return detail::_ttable.find((bits96) zob.get_hash()) != detail::_ttable.end();
}

const TTEntry &TTable::get(const ZobristHash &zob) {
    return detail::_ttable[(bits96) zob.get_hash()];
}

void TTable::clear() { detail::_ttable.clear(); }