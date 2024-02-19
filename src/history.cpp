#include "history.hpp"
#include "board.hpp"
#include <cstring>

int16_t History::hidden::_counter = 0;
HistoryNode History::hidden::_history[MAX_MOVES];

void History::init() {
    hidden::_counter = 0;
    std::memset(hidden::_history, 0, sizeof(hidden::_history));
}
int16_t History::get_ply() {
    return hidden::_counter;
}
void History::set_ply(int16_t ply) {
    hidden::_counter = ply;
}

void History::add_and_inc(const ZobristHash &zob_hash, uint8_t ply, uint8_t ep, uint8_t castling_rights) {
    hidden::_history[hidden::_counter++] = {zob_hash.get_hash(), ply, ep, castling_rights};
}
const HistoryNode &History::get_and_dec() {
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
