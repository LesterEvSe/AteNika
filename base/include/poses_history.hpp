#ifndef CHESSAI_POSES_HISTORY_HPP
#define CHESSAI_POSES_HISTORY_HPP

#include "zobrist_hash.hpp"
#include <unordered_map>

class PosesHistory {
private:
    std::unordered_map<uint64_t, uint8_t> m_poses;

public:
    PosesHistory() = default;

    // Use if an imminent move has occurred. For example, the move of pawns
    void clear();

    void add_pos(const ZobristHash& zob_hash);
    uint8_t get_repetition(const ZobristHash& zob_hash);
};


#endif //CHESSAI_POSES_HISTORY_HPP
