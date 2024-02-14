#include "history.hpp"
#include "board.hpp"

uint16_t History::hidden::_counter = 0;
HistoryNode History::hidden::_history[MAX_MOVES];

void History::add(const ZobristHash &zob_hash, Move move, uint8_t ply, uint8_t ep, uint8_t castling_rights) {
    hidden::_history[hidden::_counter].zob_hash = zob_hash;
    hidden::_history[hidden::_counter].move = move;
    hidden::_history[hidden::_counter].ply = ply;
    hidden::_history[hidden::_counter].ep = ep;
    hidden::_history[hidden::_counter++].castling_rights = castling_rights;
}
HistoryNode &History::get_and_dec() {
    return hidden::_history[--hidden::_counter];
}
bool History::threefold_rule(const Board &board) {
    bits96 hash = (bits96)board.get_zob_hash().get_hash();
    uint8_t repetitions = 0;
    uint16_t ind = hidden::_counter-1;

    for (uint8_t ply = board.get_ply(); ply > 0; --ply)
        if ((bits96)hidden::_history[ind--].zob_hash.get_hash() == hash)
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