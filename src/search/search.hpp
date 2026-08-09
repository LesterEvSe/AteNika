#pragma once

#include <atomic>
#include <chrono>
#include <string>

#include "search/order_info.hpp"

namespace Search {
  // Depth cap for a "go" that names no depth. Deliberately far below MAX_PLY
  // (100), which also sizes the killer arrays: check extensions are still
  // unbounded (Phase 1.4), so a deep search can index past their end. This
  // engine reaches ~depth 8 at blitz time controls, so the cap never binds in
  // a game — it only bounds "go infinite".
  constexpr int16_t MAX_SEARCH_DEPTH = 32;

  // Everything one "go" can constrain. Zero means "not given"; set_limits
  // resolves the combination into the flat limits the search actually reads.
  struct Limits {
    int32_t time[2]{0, 0}; // wtime / btime, indexed by Color
    int32_t inc[2]{0, 0};  // winc / binc, indexed by Color
    int32_t movetime = 0;
    int32_t movestogo = 0;
    int16_t depth = 0;
    int64_t nodes = 0;
    bool infinite = false;
  };

  void init();
  void stop(); // stop search if time has expired

  void set_time(int32_t ms_allocated);
  void set_depth(int16_t depth);
  void set_max_nodes(int64_t nodes); // 0 disables the node limit
  void set_limits(const Limits &limits, Color side_to_move);

  // Extra diagnostics on the "info string" channel, toggled by UCI "debug".
  void set_debug(bool on);

  [[nodiscard]] std::string get_mate();
  [[nodiscard]] Move *get_best_move();
  [[nodiscard]] std::string get_allocated_sec();
  [[nodiscard]] int16_t get_search_depth();

  // Needed by bench, which reports node counts and has to save and restore
  // the user's limits around a run.
  [[nodiscard]] int64_t get_nodes();
  [[nodiscard]] int32_t get_allocated_ms();
  [[nodiscard]] bool is_without_time();
  [[nodiscard]] int64_t get_max_nodes();
  [[nodiscard]] int16_t get_seldepth();

  void iter_deep(Board &board, bool print_info);
} // namespace Search

namespace Search::detail {
  // Limits
  extern int64_t _nodes;
  extern int32_t _ms_allocated;
  extern bool _without_time;
  extern int16_t _depth;
  extern int64_t _max_nodes;
  extern std::atomic<bool> _stop;

  extern int64_t _fh;  // cut-off at n move. The moves are accumulating
  extern int64_t _fhf; // cut-off at first move

  // Search
  extern OrderInfo _order_info;
  extern Move _best_move;
  extern int32_t _best_score;
  extern int16_t _seldepth;
  extern bool _debug_info;
  extern std::chrono::time_point<std::chrono::steady_clock> _start;

  extern std::string _mate; // for mate check

  void _info(const Board &board, int depth, int elapsed);
  void _restart();
  bool _check_limits();

  int32_t _negamax(Board &board, int16_t depth, int32_t alpha, int32_t beta, bool null_move);
  int32_t _quiescence(Board &board, int32_t alpha, int32_t beta);

} // namespace Search::detail
