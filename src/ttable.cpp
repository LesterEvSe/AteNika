#include "ttable.hpp"

std::unordered_map<bits96, TTEntry> Ttable::hidden::_ttable;

void Ttable::add(const ZobristHash &zob, TTEntry entry) {
    hidden::_ttable[(bits96)zob.get_hash()] = entry;
}

bool Ttable::in_table(const ZobristHash &zob) {
    return hidden::_ttable.find((bits96)zob.get_hash()) != hidden::_ttable.end();
}

const TTEntry &Ttable::get(const ZobristHash &zob) {
    return hidden::_ttable[(bits96)zob.get_hash()];
}