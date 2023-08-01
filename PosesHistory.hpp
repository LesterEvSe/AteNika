#ifndef CHESSAI_POSESHISTORY_HPP
#define CHESSAI_POSESHISTORY_HPP

#include "zobrist_hash.hpp"
#include <unordered_map>

class PosesHistory {
private:
    static std::unordered_map<uint64_t, uint8_t> s_poses;

public:
    PosesHistory() = delete;
    PosesHistory(PosesHistory&&)      = delete;
    PosesHistory(const PosesHistory&) = delete;
    PosesHistory& operator=(PosesHistory&&)       = delete;
    PosesHistory& operator=(const PosesHistory&)  = delete;

    // Use if an imminent move has occurred. For example, the move of pawns
    static void clear();
    static void add_pos(ZobristHash zob_hash);
    static uint8_t get_repetition(ZobristHash zob_hash);
};


#endif //CHESSAI_POSESHISTORY_HPP
