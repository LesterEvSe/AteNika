#include "ttable.hpp"

std::unordered_map<bits96, Move> Ttable::hidden::_ttable;

void Ttable::add(const ZobristHash &zob, Move move) {
    hidden::_ttable[(bits96)zob.get_hash()] = move;
}

bool Ttable::in_table(const ZobristHash &zob) {
    return hidden::_ttable.find((bits96)zob.get_hash()) != hidden::_ttable.end();
}

Move Ttable::get(const ZobristHash &zob) {
    return hidden::_ttable[(bits96)zob.get_hash()];
}