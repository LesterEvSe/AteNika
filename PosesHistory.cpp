#include "PosesHistory.hpp"

std::unordered_map<uint64_t, uint8_t> PosesHistory::s_poses;

void PosesHistory::clear() {
    s_poses.clear();
}
void PosesHistory::add_pos(ZobristHash zob_hash) {
    ++s_poses[zob_hash.get_hash()];
}
uint8_t PosesHistory::get_repetition(ZobristHash zob_hash) {
    return s_poses[zob_hash.get_hash()];
}
