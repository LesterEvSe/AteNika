#pragma once

#include <cstdint>
#include <string>

#include "core/move.hpp"
#include "defs.hpp"

class Board;

namespace Search {
  // Depth cap for a "go" that names no depth. Deliberately far below MAX_PLY
  // (100), which also sizes the killer arrays: check extensions are still
  // unbounded (Phase 1.4), so a deep search can index past their end. This
  // engine reaches ~depth 8 at blitz time controls, so the cap never binds in
  // a game; it only bounds "go infinite".
  constexpr int16_t MAX_SEARCH_DEPTH = 32;

  // Everything one "go" can constrain. Zero means "not given"; set_limits
  // resolves the combination into the flat limits the search actually reads.
  struct Limits {
    int32_t time[COLOR_SIZE]{0, 0}; // wtime / btime
    int32_t inc[COLOR_SIZE]{0, 0};  // winc / binc
    int32_t movetime{0};
    int32_t movestogo{0};
    int16_t depth{0};
    int64_t nodes{0};
    bool infinite{false};
  };

  void init();

  // Driving a search. This is the whole surface UCI needs: resolve one "go"
  // into limits, run it, interrupt it, read the answer back.
  void set_limits(const Limits &limits, Color side_to_move);
  void iter_deep(Board &board, bool print_info);
  void stop();
  [[nodiscard]] Move *get_best_move();
  void set_debug(bool on); // extra "info string" output, UCI "debug on"

  [[nodiscard]] int64_t get_nodes();
  [[nodiscard]] int16_t get_seldepth();
  [[nodiscard]] std::string get_mate(); // mate tests only

  // Individual limits. bench and the mate tests set these directly instead of
  // going through set_limits.
  void set_time(int32_t ms_allocated);
  void set_depth(int16_t depth);
  void set_max_nodes(int64_t nodes); // 0 disables the node limit

  [[nodiscard]] int32_t get_allocated_ms();
  [[nodiscard]] bool is_without_time();
  [[nodiscard]] int16_t get_search_depth();
  [[nodiscard]] int64_t get_max_nodes();
  [[nodiscard]] std::string get_allocated_sec(); // human-readable, for "state"
} // namespace Search
