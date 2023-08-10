#include "poses_history.hpp"

void PosesHistory::clear() {
    m_poses.clear();
}
void PosesHistory::add_pos(const ZobristHash& zob_hash) {
    ++m_poses[zob_hash.get_hash()];
}
uint8_t PosesHistory::get_repetition(const ZobristHash& zob_hash) {
    return m_poses[zob_hash.get_hash()];
}
