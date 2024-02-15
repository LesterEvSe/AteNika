#include "history.hpp"
#include "board.hpp"
#include <cstring>

int16_t History::hidden::_counter = 0;
HistoryNode History::hidden::_history[MAX_MOVES];

void History::init() {
    hidden::_counter = 0;
    std::memset(hidden::_history, 0, sizeof(hidden::_history));
}

void History::add_and_inc(const ZobristHash &zob_hash, uint8_t ply, uint8_t ep, uint8_t castling_rights) {
//    std::cout << 'i' << (int)hidden::_counter << ' ';
//    hidden::_history[hidden::_counter++] = {zob_hash, move, ply, ep, castling_rights}; // more readable view

    hidden::_history[hidden::_counter].hash = zob_hash.get_hash();
    hidden::_history[hidden::_counter].ply = ply;
    hidden::_history[hidden::_counter].ep = ep;
    hidden::_history[hidden::_counter++].castling_rights = castling_rights;
}
const HistoryNode &History::get_and_dec() {
//    std::cout << 'd' << (int)hidden::_counter << ' ';
    return hidden::_history[--hidden::_counter];
}
bool History::threefold_rule(const Board &board) {
    uint96 hash = board.get_zob_hash().get_hash();
    uint8_t repetitions = 0;
    uint16_t ind = hidden::_counter-1;

    for (uint8_t ply = board.get_ply(); ply > 0; --ply)
        if (hidden::_history[ind--].hash == hash)
            ++repetitions;
    return repetitions >= 3;
}
void History::clear() {
    hidden::_counter = 0;
}

/*
void History::operator=(const History &history) {
    m_size = history.m_size;
    for (uint8_t i = 0; i < m_size; ++i)
        m_history[i] = history.m_history[i];
}

void History::clear() {
    m_size = 0;
}

bool History::add_pos(const ZobristHash &zob_hash) {
    if (m_size >= MAX_PLY) return false;
    m_history[m_size++] = (bits96)zob_hash.get_hash();
    return true;
}

bool History::threefold_repetition(const ZobristHash &zob_hash) const {
    uint8_t repetitions = 0;
    bits96 hash = (bits96)zob_hash.get_hash();

    for (uint8_t i = 0; i < m_size; ++i)
        if (m_history[i] == hash)
            ++repetitions;
    return repetitions >= 3;
}
*/