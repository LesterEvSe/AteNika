#ifndef ATENIKA_SEARCH_HPP
#define ATENIKA_SEARCH_HPP

#include "board.hpp"
#include "history.hpp"
#include <chrono>
#include <atomic>

namespace Search {
namespace hidden {
    constexpr int16_t GAP = 5000;

    extern int64_t _nodes;
    extern int32_t _best_score;
    extern Move _best_move;
    extern History _history; // TODO fix bug with empty history

    extern int32_t _time_allocated_ms;
    extern bool _without_time;

    extern int16_t _check_gap;
    extern std::chrono::time_point<std::chrono::steady_clock> _start;

    extern std::atomic<bool> _stop; // if time expired, then false
    extern int16_t _seeking_depth; // Want depth = 20. Unfortunately too much

    int32_t _negamax(const Board &board, int16_t depth, int32_t alpha, int32_t beta);
    int32_t _quiescence_search(const Board &board, int32_t alpha, int32_t beta);

    void new_search();
    bool check_limits();

} // hidden
    void init();
    void stop(); // stop search if time has expired
    bool set_time(int32_t time_allocated_ms);
    bool set_depth(int16_t n);

    Move *get_best_move();
    int32_t get_score();
    void iter_deep(const Board &board, bool debug = false);
}

#endif //ATENIKA_SEARCH_HPP
