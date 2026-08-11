// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include "search/bench.hpp"

#include <chrono>
#include <cstdio>
#include <print>

#include "core/board.hpp"
#include "search/search.hpp"
#include "search/ttable.hpp"

namespace {

  // Chosen to exercise different code paths rather than to be hard: openings and
  // closed middlegames, tactical positions, promotions, en passant, castling
  // rights, and pawn endgames where the endgame king tables take over. A change
  // confined to one part of the evaluation still has to show up somewhere here.
  const char *POSITIONS[] = {
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
      "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
      "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
      "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 0 1",
      "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 1",
      "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0 1",
      "3r1rk1/p3qp1p/2bb2p1/2p5/3P4/1P6/PBQN1PPP/2R2RK1 w - - 0 1",
      "rnbqkb1r/pp1p1ppp/2p5/4P3/2B5/8/PPP1NnPP/RNBQK2R w KQkq - 0 1",
      "4rrk1/pp1n1pp1/3bp2p/3p4/3P4/1P1B1N2/P1P2PPP/2R1R1K1 w - - 0 1",
      "8/8/8/8/5kp1/P7/8/1K1N4 w - - 0 1",
      "8/2p5/8/2P5/8/1k6/8/1K6 w - - 0 1",
      "6k1/6p1/7p/P1N5/1r3p2/7P/1b3PP1/4R1K1 w - - 0 1",
      "8/3k4/8/8/8/8/4P3/4K3 w - - 0 1",
      "r1bq1rk1/pp2ppbp/2np1np1/8/3NP3/2N1BP2/PPPQ2PP/R3KB1R w KQ - 0 1",
  };

  constexpr int POSITION_COUNT = sizeof(POSITIONS) / sizeof(POSITIONS[0]);

} // namespace

int64_t Bench::run(int depth) {
  // Save whatever the user configured; bench must not leave the engine in a
  // different state than it found it.
  const int32_t saved_ms = Search::get_allocated_ms();
  const bool saved_without_time = Search::is_without_time();
  const int16_t saved_depth = Search::get_search_depth();
  const int64_t saved_max_nodes = Search::get_max_nodes();

  // INF disables both time exits: the abort inside _check_limits and the
  // half-budget break in iter_deep. Without this the node count would depend
  // on how fast the machine happened to be.
  Search::set_time(INF);
  Search::set_depth(static_cast<int16_t>(depth));

  // Likewise for the node limit: a "go nodes 10000" earlier in the session
  // would otherwise silently truncate every position in the run.
  Search::set_max_nodes(0);

  int64_t total_nodes = 0;
  const auto start = std::chrono::steady_clock::now();

  for (int i = 0; i < POSITION_COUNT; ++i) {
    // Each position starts from an empty table, so the result depends only
    // on the binary and not on the order positions happen to run in.
    TTable::clear();

    Board board(POSITIONS[i]);
    Search::iter_deep(board, false);

    const int64_t nodes = Search::get_nodes();
    total_nodes += nodes;

    Move *best = Search::get_best_move();
    std::println("Position {}/{}  nodes {}  best {}", i + 1, POSITION_COUNT, nodes,
                 best ? static_cast<std::string>(*best) : "none");
    // Progress is only useful live, and stdout is fully buffered on a pipe.
    std::fflush(stdout);
  }

  const int64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::steady_clock::now() - start)
                         .count();

  std::println("\n===========================");
  std::println("Depth          : {}", depth);
  std::println("Positions      : {}", POSITION_COUNT);
  std::println("Nodes searched : {}", total_nodes);
  std::println("Time           : {} ms", ms);
  std::println("NPS            : {}", total_nodes * 1000 / (ms + 1));

  TTable::clear();
  Search::set_depth(static_cast<int16_t>(saved_depth));
  Search::set_time(saved_without_time ? INF : saved_ms);
  Search::set_max_nodes(saved_max_nodes);

  return total_nodes;
}
