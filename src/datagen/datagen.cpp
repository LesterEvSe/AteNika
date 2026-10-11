// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include "datagen/datagen.hpp"

#include <algorithm>
#include <bit>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <print>
#include <random>
#include <utility>
#include <vector>

#include "core/board.hpp"
#include "eval/eval.hpp"
#include "movegen/movegen.hpp"
#include "search/search.hpp"
#include "search/ttable.hpp"

namespace {
  constexpr int RANDOM_PLIES = 8;
  constexpr int32_t OPENING_MAX_SCORE = 1000;

  constexpr int MIN_RECORD_PLY = 16;
  constexpr int MAX_GAME_PLY = 400;

  constexpr int32_t WIN_SCORE = 2000;
  constexpr int WIN_PLIES = 4;

  constexpr int DRAW_MIN_PLY = 160;
  constexpr int32_t DRAW_SCORE = 10;
  constexpr int DRAW_PLIES = 8;

  // bulletformat stores the score as i16.
  constexpr int32_t SCORE_CLAMP = 32000;

  constexpr int64_t REPORT_EVERY_GAMES = 100;

  struct Stats {
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    int64_t searches = 0;
    int64_t nodes = 0;
    int64_t depth = 0;

    void add() {
      ++searches;
      nodes += Search::get_nodes();
      depth += Search::get_completed_depth();
    }

    void report(const char *what, int64_t games, int64_t positions) const {
      const double seconds =
          std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
      const double n = static_cast<double>(std::max<int64_t>(searches, 1));

      std::println(stderr, "{}: games {} positions {} depth {:.2f} nodes {:.0f} pos/s {:.0f}", what,
                   games, positions, static_cast<double>(depth) / n, static_cast<double>(nodes) / n,
                   static_cast<double>(positions) / seconds);
    }
  };

  // The table only has to hold one search, so its size follows the budget.
  void set_limits(int64_t nodes) {
    Search::set_time(INF);
    Search::set_depth(Search::MAX_SEARCH_DEPTH);
    Search::set_max_nodes(nodes);
    TTable::resize(
        std::bit_ceil(static_cast<size_t>(std::max<int64_t>(nodes * 32 / 1'048'576, 1))));
  }

  int32_t clamp_score(int32_t score) { return std::clamp(score, -SCORE_CLAMP, SCORE_CLAMP); }

  const char *result_text(double result) {
    if (result > 0.75)
      return "1.0";
    return result < 0.25 ? "0.0" : "0.5";
  }

  bool play_opening(Board &board, std::mt19937_64 &rng) {
    for (int i = 0; i < RANDOM_PLIES; ++i) {
      Movegen movegen(board);
      MoveList &moves = movegen.get_legal_moves();
      if (moves.size() == 0)
        return false;

      std::uniform_int_distribution<int> pick(0, moves.size() - 1);
      const Move move = moves[static_cast<uint8_t>(pick(rng))];
      board.make(move);
    }
    return true;
  }

  // Returns false when the game is thrown away. `result` is from White's side.
  bool play_game(Board &board, std::vector<std::pair<std::string, int32_t>> &records,
                 double &result, Stats &stats) {
    int win_plies = 0;
    int draw_plies = 0;

    for (int ply = RANDOM_PLIES;; ++ply) {
      Movegen movegen(board);
      const bool in_check = movegen.in_check();
      const Color us = board.get_curr_move();

      if (movegen.get_legal_moves().size() == 0) {
        result = !in_check ? 0.5 : us == WHITE ? 0.0 : 1.0;
        return true;
      }
      if (ply >= MAX_GAME_PLY || board.threefold_rule() || board.get_ply() >= MAX_PLY ||
          Eval::material_draw(board)) {
        result = 0.5;
        return true;
      }

      Search::iter_deep(board, false);
      stats.add();

      const Move *best = Search::get_best_move();
      if (best == nullptr)
        return false;

      const int32_t score = Search::get_best_score();
      const bool mate = Search::is_mate_score(score);
      const int32_t white_score = us == WHITE ? score : -score;

      if (ply == RANDOM_PLIES && std::abs(white_score) > OPENING_MAX_SCORE)
        return false;

      if (ply >= MIN_RECORD_PLY && !in_check && !best->is_tactical() && !mate)
        records.emplace_back(board.get_fen(), white_score);

      if (mate) {
        result = white_score > 0 ? 1.0 : 0.0;
        return true;
      }

      win_plies = std::abs(white_score) >= WIN_SCORE ? win_plies + 1 : 0;
      if (win_plies >= WIN_PLIES) {
        result = white_score > 0 ? 1.0 : 0.0;
        return true;
      }

      draw_plies = ply >= DRAW_MIN_PLY && std::abs(white_score) <= DRAW_SCORE ? draw_plies + 1 : 0;
      if (draw_plies >= DRAW_PLIES) {
        result = 0.5;
        return true;
      }

      board.make(*best);
    }
  }
} // namespace

void Datagen::generate(int64_t nodes, int64_t positions, uint64_t seed, const std::string &path) {
  std::ofstream out(path);
  if (!out) {
    std::println(stderr, "datagen: cannot write {}", path);
    return;
  }

  set_limits(nodes);
  std::mt19937_64 rng(seed);
  Stats stats;

  int64_t written = 0;
  int64_t games = 0;
  std::vector<std::pair<std::string, int32_t>> records;

  while (written < positions) {
    TTable::clear();
    Search::new_game();

    Board board;
    records.clear();
    double result = 0.5;

    if (!play_opening(board, rng) || !play_game(board, records, result, stats))
      continue;

    ++games;
    for (const auto &[fen, score] : records) {
      if (written == positions)
        break;
      std::println(out, "{} | {} | {}", fen, clamp_score(score), result_text(result));
      ++written;
    }

    if (games % REPORT_EVERY_GAMES == 0)
      stats.report("datagen", games, written);
  }
  stats.report("datagen done", games, written);
}

void Datagen::relabel(int64_t nodes, const std::string &in, const std::string &out) {
  std::ifstream input(in);
  std::ofstream output(out);
  if (!input || !output) {
    std::println(stderr, "relabel: cannot open {} or {}", in, out);
    return;
  }

  set_limits(nodes);
  Stats stats;

  int64_t written = 0;
  std::string line;

  while (std::getline(input, line)) {
    const size_t first = line.find('|');
    const size_t last = line.rfind('|');
    if (first == std::string::npos || first == last) {
      std::println(stderr, "relabel: skipping malformed line {}", line);
      continue;
    }

    const std::string fen = line.substr(0, line.find_last_not_of(' ', first - 1) + 1);
    const std::string result = line.substr(last + 1);

    TTable::clear();
    Search::new_game();

    Board board(fen);
    Search::iter_deep(board, false);
    stats.add();

    const int32_t score = Search::get_best_score();
    const int32_t white_score = board.get_curr_move() == WHITE ? score : -score;
    std::println(output, "{} | {} |{}", fen, clamp_score(white_score), result);

    if (++written % (REPORT_EVERY_GAMES * 100) == 0)
      stats.report("relabel", 0, written);
  }
  stats.report("relabel done", 0, written);
}
