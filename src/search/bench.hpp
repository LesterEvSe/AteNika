#pragma once

#include <cstdint>

// A fixed set of positions searched to a fixed depth, reported as one total node
// count. The number is a fingerprint of the whole search: move ordering, pruning,
// transposition hits and evaluation scores all feed into it, so any behavioural
// change anywhere moves it.
//
// A refactor must leave the node count identical. An optimization or a new
// heuristic is expected to change it. That distinction is the entire point.
//
// Deliberately depth-limited, never time-limited: a time limit would make the
// result depend on CPU speed, thermal state and background load.
namespace Bench {

  // ~4 s today. Raise it once the search cheaper per ply;
  // changing it changes the number, so treat a depth change as its own commit.
  constexpr int DEFAULT_DEPTH = 7;

  // Returns the total node count so callers can use it programmatically.
  int64_t run(int depth = DEFAULT_DEPTH);

} // namespace Bench
