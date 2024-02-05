#include "ttable.hpp"

std::unordered_map<bits96, Move> Ttable::hidden::_ttable;

void Ttable::add(const ZobristHash &zob, Move move) {
    hidden::_ttable[(bits96)zob.get_hash()] = move;
}

Move Ttable::get(const ZobristHash &zob) {
    return hidden::_ttable[(bits96)zob.get_hash()];
}