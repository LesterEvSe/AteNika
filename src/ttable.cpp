#include "ttable.hpp"

std::unordered_map<bits96, TTEntry> TTable::hidden::_ttable;

void TTable::add(const ZobristHash &zob, TTEntry entry) {
    hidden::_ttable[(bits96)zob.get_hash()] = entry;
}

bool TTable::in_table(const ZobristHash &zob) {
    return hidden::_ttable.find((bits96)zob.get_hash()) != hidden::_ttable.end();
}

const TTEntry &TTable::get(const ZobristHash &zob) {
    return hidden::_ttable[(bits96)zob.get_hash()];
}