// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include "search/search.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath> // for std::log in lmr
#include <cstdio>
#include <format>
#include <print>
#include <string>

#include "core/board.hpp"
#include "eval/eval.hpp"
#include "movegen/movegen.hpp"
#include "search/movepicker.hpp"
#include "search/order_info.hpp"
#include "search/see.hpp"
#include "search/ttable.hpp"

namespace {
  // _negamax returns -INF + ply when the side to move is mated, so a score this
  // large is a forced mate and INF - |score| is its distance in plies.
  constexpr int32_t MATE_BOUND = 2'000'000'000;

  // Held back from every time budget. Covers pipe latency, process scheduling
  // and the 2048-node granularity of _check_limits.
  constexpr int32_t MOVE_OVERHEAD_MS = 50;

  constexpr int16_t ASPIRATION_MIN_DEPTH = 4;
  constexpr int32_t ASPIRATION_DELTA = 25;

  // Time management. The soft limit decides whether to open another iteration,
  // the hard limit aborts one already running.
  constexpr int32_t DEFAULT_MOVES_LEFT = 20;
  constexpr int32_t MAX_MOVES_LEFT = 30;

  // Share of the move's allocation to spend before declining to open another
  // iteration. This was an unnamed 0.5; the games above are what argue for
  // raising it.
  constexpr double SOFT_USE_FRACTION = 0.62;

  // Bounds how wrong the prediction is allowed to be.
  constexpr int32_t HARD_LIMIT_FACTOR = 3;
  constexpr int32_t HARD_LIMIT_CLOCK_SHARE = 5;

  // A best move that never moves is the ordinary case, not a signal to hurry, so
  // stability only trims and it trims gently.
  constexpr int16_t STABILITY_CAP = 8;
  constexpr double STABILITY_STEP = 0.02;
  constexpr int32_t PANIC_SCORE_DROP = 40;
  constexpr double PANIC_BONUS = 0.4;
  constexpr double SOFT_SCALE_MIN = 0.80;
  constexpr double SOFT_SCALE_MAX = 1.50;

  // Cumulative-elapsed ratio, not per-iteration: the per-iteration series is far
  // noisier and predicted the measured timings worse.
  constexpr double EBF_MIN = 1.4;
  constexpr double EBF_MAX = 3.5;

  // Currently skipped because has no gain from it.
  // https://www.chessprogramming.org/Futility_Pruning#Move_Count_Based_Pruning
  // Late move pruning, 3 + depth * depth. Depth 1 is skipped on purpose: measured
  // first-move cutoff rate there is 64-89%, against 90%+ from depth 2 on, and it
  // carries more fail-highs than every other depth combined.

  // These two prove itself as +118 +/- 35 with sprt.
  // https://www.chessprogramming.org/Reverse_Futility_Pruning or static null move pruning.
  constexpr int16_t RFP_MAX_DEPTH = 6;
  constexpr int32_t RFP_MARGIN = 80;

  // https://www.chessprogramming.org/Futility_Pruning
  constexpr int16_t FUTILITY_MAX_DEPTH = 3;
  constexpr int32_t FUTILITY_MARGIN = 100;

  // https://www.chessprogramming.org/Null_Move_Pruning#Adaptive_Null_Move_Pruning
  constexpr int16_t NULL_MOVE_MIN_DEPTH = 4;
  constexpr int16_t NULL_MOVE_BASE_R = 3;
  constexpr int16_t NULL_MOVE_DEPTH_DIV = 6;
  constexpr int32_t NULL_MOVE_EVAL_DIV = 200;
  constexpr int32_t NULL_MOVE_MAX_R = 3;

  // https://www.chessprogramming.org/Delta_Pruning
  constexpr int32_t DELTA_MARGIN = 200;

  // History-based reduction.
  constexpr int32_t LMR_HISTORY_DIV = 2'048;
  constexpr int16_t LMR_HISTORY_MAX = 2;

  // UCI reports mate distance in moves, signed from the side to move: positive
  // when we deliver it, negative when we are the one being mated. The search
  // counts plies, hence the halving.
  std::string format_score(int32_t score) {
    if (score > MATE_BOUND)
      return std::format("mate {}", (INF - score + 1) / 2);
    if (score < -MATE_BOUND)
      return std::format("mate -{}", (INF + score + 1) / 2);
    return std::format("cp {}", score);
  }

  // Score here is the distance to mate, counted in plies.
  //
  // Clamped: UBSan caught a signed overflow, because the score can be a beta
  // value carried down from another branch, not just a mate found below.
  int32_t _score_to_tt(int32_t score, int16_t ply) {
    if (score > MATE_BOUND)
      return static_cast<int32_t>(std::min<int64_t>(int64_t{score} + ply, INF));
    if (score < -MATE_BOUND)
      return static_cast<int32_t>(std::max<int64_t>(int64_t{score} - ply, -INF));
    return score;
  }

  int32_t _score_from_tt(int32_t score, int16_t ply) {
    if (score > MATE_BOUND)
      return score - ply;
    if (score < -MATE_BOUND)
      return score + ply;
    return score;
  }
} // namespace

namespace Search::detail {
  // Limits
  int64_t _nodes;
  int32_t _ms_allocated; // hard limit, enforced inside the search by _check_limits
  int32_t _soft_limit;   // consulted only between iterations
  bool _without_time;
  int16_t _depth;
  int64_t _max_nodes;
  std::atomic<bool> _stop;

  // Split by search for more precise information.
  // *Searched* move, not *Generated* one.
  int64_t _fh;    // main search: cut-offs
  int64_t _fhf;   // main search: cut-offs on the first searched move
  int64_t _q_fh;  // quiescence: cut-offs
  int64_t _q_fhf; // quiescence: cut-offs on the first searched move

  // Search
  OrderInfo _order_info;
  Move _best_move;
  int32_t _best_score;
  int16_t _seldepth;

  int16_t _root_depth;
  bool _debug_info;
  std::chrono::time_point<std::chrono::steady_clock> _start;

  std::string _mate; // for mate check

  // Triangular PV table. Row p is the line from ply p onwards, written from
  // index p so a parent copies its child's row straight across:
  //
  //             0      1      2      3     _pv_length
  //    row 0   e2e4   e7e5   g1f3   b8c6        4
  //    row 1    -     e7e5   g1f3   b8c6        4
  //    row 2    -      -     g1f3   b8c6        4
  //    row 3    -      -      -     b8c6        4
  //
  // _pv_length[p] is an end index, not a count: row p spans [p, length). So row
  // 0 reads out as the whole line and _pv[0][0] is the move sent as bestmove.
  Move _pv[MAX_SEARCH_PLY][MAX_SEARCH_PLY];
  int16_t _pv_length[MAX_SEARCH_PLY];

  // Row 0 of the last *completed* iteration. An aborted iteration leaves _pv[0]
  // half-built, so it never reaches here.
  Move _best_pv[MAX_SEARCH_PLY];
  int16_t _best_pv_length;

  // https://chessprogramming.org/Late_Move_Reductions
  // Setup max moves to 64, because very unlickely that we reach more than 64 moves
  // During LMR, so just do it that way, instead of refering
  // to the 218 (max possible moves in position)
  int16_t _lmr[MAX_SEARCH_DEPTH + 1][64];

  // Declared ahead of the definitions below: iter_deep calls into them before
  // they appear, and _negamax and _quiescence are mutually recursive.
  void _info(int depth, int elapsed);
  void _restart();
  bool _check_limits();

  // More here: https://chessprogramming.org/Negamax
  int32_t _negamax(Board &board, int16_t depth, int32_t alpha, int32_t beta, bool null_move);

  // More here: https://chessprogramming.org/Quiescence_Search
  int32_t _quiescence(Board &board, int32_t alpha, int32_t beta);
} // namespace Search::detail

void Search::detail::_restart() {
  _nodes = 0;
  _stop = false;

  _fh = 0;
  _fhf = 0;
  _q_fh = 0;
  _q_fhf = 0;
  _mate = "";
  _seldepth = 0;
  _root_depth = 0;
  _best_score = 0;
  _best_pv_length = 0;

  _order_info.new_search();
  _best_move = Move();
}

void Search::new_game() { detail::_order_info.new_game(); }

void Search::init() {
  detail::_order_info.new_game();
  detail::_restart();
  detail::_ms_allocated = 5000;
  detail::_soft_limit = 5000;
  detail::_without_time = false;
  detail::_depth = 10;
  detail::_max_nodes = 0;
  detail::_debug_info = false;

  // Currently leave as it is. Tune it later.
  for (int d = 1; d <= MAX_SEARCH_DEPTH; ++d)
    for (int m = 1; m < 64; ++m)
      detail::_lmr[d][m] = static_cast<int16_t>(0.75 + std::log(d) * std::log(m) / 2.25);
}

bool Search::detail::_check_limits() {
  if (_stop)
    return true;
  // check each 2048 node
  if (_nodes & 2047)
    return false;

  // Shares the clock's 2048-node cadence, so "go nodes N" lands within a couple
  // of thousand nodes of N rather than exactly on it. Zero disables it, which
  // is what keeps bench independent of whatever the last "go" asked for.
  if (_max_nodes > 0 && _nodes >= _max_nodes)
    return _stop = true;

  auto elapsed = static_cast<int32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                                          std::chrono::steady_clock::now() - detail::_start)
                                          .count());

  if (_without_time || elapsed < _ms_allocated)
    return false;
  return _stop = true;
}
void Search::stop() { detail::_stop = true; }

std::string Search::get_mate() { return detail::_mate; }
Move *Search::get_best_move() {
  return detail::_best_move.get_flag() == Move::NULL_MOVE ? nullptr : &detail::_best_move;
}
std::string Search::get_allocated_sec() {
  return detail::_without_time ? "infinity" : std::to_string(detail::_ms_allocated / 1000) + " sec";
}
int16_t Search::get_search_depth() { return detail::_depth; }

int64_t Search::get_nodes() { return detail::_nodes; }
int32_t Search::get_allocated_ms() { return detail::_ms_allocated; }
bool Search::is_without_time() { return detail::_without_time; }
int64_t Search::get_max_nodes() { return detail::_max_nodes; }
int16_t Search::get_seldepth() { return detail::_seldepth; }

const Move *Search::get_pv() { return detail::_best_pv; }
int16_t Search::get_pv_length() { return detail::_best_pv_length; }

void Search::set_time(int32_t ms_allocated) {
  if (ms_allocated == INF)
    detail::_without_time = true;
  else if (ms_allocated > 0) {
    detail::_without_time = false;
    detail::_ms_allocated = ms_allocated;
    detail::_soft_limit = ms_allocated;
  }
}

void Search::set_depth(int16_t depth) {
  if (depth > 0)
    detail::_depth = depth;
}

void Search::set_max_nodes(int64_t nodes) { detail::_max_nodes = nodes > 0 ? nodes : 0; }
void Search::set_debug(bool on) { detail::_debug_info = on; }

void Search::set_limits(const Limits &limits, Color side_to_move) {
  detail::_max_nodes = limits.nodes > 0 ? limits.nodes : 0;
  detail::_depth = limits.depth > 0 ? limits.depth : MAX_SEARCH_DEPTH;

  if (limits.infinite || (limits.movetime <= 0 && limits.time[side_to_move] <= 0)) {
    detail::_without_time = true;
    return;
  }

  detail::_without_time = false;
  if (limits.movetime > 0) {
    // "Think for exactly this long" leaves nothing to allocate, so both limits
    // are the same and only the iteration prediction decides when to stop.
    detail::_ms_allocated = limits.movetime;
    detail::_soft_limit = limits.movetime;
    return;
  }

  const int32_t remaining = limits.time[side_to_move];
  const int32_t moves_left =
      limits.movestogo > 0 ? std::min(limits.movestogo, MAX_MOVES_LEFT) : DEFAULT_MOVES_LEFT;

  const int32_t alloc = remaining / moves_left + limits.inc[side_to_move] * 3 / 4;
  int32_t soft = static_cast<int32_t>(alloc * SOFT_USE_FRACTION);
  int32_t hard = std::min(remaining / HARD_LIMIT_CLOCK_SHARE, alloc * HARD_LIMIT_FACTOR);

  // Never plan to spend the whole clock. Pipe latency, process scheduling and
  // the 2048-node granularity of _check_limits all overshoot a little, and
  // flagging loses the game outright no matter how good the move was.
  const int32_t cap = std::max(remaining - MOVE_OVERHEAD_MS, 1);
  hard = std::clamp(hard, 1, cap);
  soft = std::clamp(soft, 1, hard);

  detail::_ms_allocated = hard;
  detail::_soft_limit = soft;
}


void Search::detail::_info(int depth, int elapsed) {
  // Sometimes we have an error in Linux.
  // Process finished with exit code 136 (interrupted by signal 8:SIGFPE)
  // It's divide by zero error, so I increment elapsed ms, to avoid this problem
  const int64_t nps = _nodes * 1000 / (elapsed + 1);

  std::string pv;
  for (int16_t i = 0; i < _best_pv_length; ++i)
    pv += static_cast<std::string>(_best_pv[i]) + ' ';

  if (!pv.empty())
    pv.pop_back();

  std::println("info depth {} seldepth {} score {} nodes {} nps {} time {} pv {}", depth, _seldepth,
               format_score(_best_score), _nodes, nps, elapsed, pv);

  // moq — move ordering quality, the share of fail-highs that resolved on the
  // first move. Read "main" to judge ordering changes (killers, history,
  // continuation history); read "qs" to judge whether staged generation in 3.3
  // is worth the work.
  if (_debug_info) {
    const auto share = [](int64_t part, int64_t total) {
      return static_cast<int>(100.0 * static_cast<double>(part) /
                              static_cast<double>(total == 0 ? 1 : total));
    };
    std::println("info string moq main {}% (fh {} fhf {}) qs {}% (fh {} fhf {})", share(_fhf, _fh),
                 _fh, _fhf, share(_q_fhf, _q_fh), _q_fh, _q_fhf);
  }

  std::fflush(stdout);
}

void Search::iter_deep(Board &board, bool print_info) {
  detail::_restart();
  detail::_start = std::chrono::steady_clock::now();

  TTable::new_search();

  int32_t prev_score = 0;
  int32_t prev_elapsed = 0;
  int16_t stability = 0;
  Move prev_best = Move();

  for (int16_t i = 1; i <= detail::_depth; ++i) {
    detail::_root_depth = i;

    int32_t alpha = -INF;
    int32_t beta = INF;
    int32_t delta = ASPIRATION_DELTA;

    if (i >= ASPIRATION_MIN_DEPTH && std::abs(prev_score) < MATE_BOUND) {
      alpha = prev_score - delta;
      beta = prev_score + delta;
    }

    int32_t score = 0;
    while (true) {
      score = detail::_negamax(board, i, alpha, beta, true);
      if (detail::_stop)
        break;

      if (score <= alpha && alpha > -INF)
        alpha = static_cast<int32_t>(std::max<int64_t>(int64_t{score} - delta, -INF));
      else if (score >= beta && beta < INF)
        beta = static_cast<int32_t>(std::min<int64_t>(int64_t{score} + delta, INF));
      else
        break;

      delta += delta / 2;
    }

    if (detail::_stop)
      break;

    // Widened, because INF - (-INF) can overflow.
    const int32_t score_drop =
        i > 1 ? static_cast<int32_t>(std::clamp<int64_t>(int64_t{prev_score} - score, -INF, INF))
              : 0;
    detail::_best_score = score;
    prev_score = score;

    // static_cast for MSVC W4 warnings
    auto elapsed = static_cast<int32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                                            std::chrono::steady_clock::now() - detail::_start)
                                            .count());

    // Only a completed iteration gets to publish its line.
    if (detail::_pv_length[0] > 0) {
      detail::_best_pv_length = detail::_pv_length[0];

      for (int16_t j = 0; j < detail::_best_pv_length; ++j)
        detail::_best_pv[j] = detail::_pv[0][j];

      detail::_best_move = detail::_best_pv[0];
    }

    if (print_info)
      detail::_info(i, elapsed);

    if (detail::_best_score > MATE_BOUND) {
      // for testing
      detail::_mate = (board.get_curr_move() == WHITE ? "WM" : "BM") +
                      std::to_string(INF - detail::_best_score);
      break;
    }

    if (detail::_best_move == prev_best)
      stability = std::min<int16_t>(stability + 1, STABILITY_CAP);
    else
      stability = 0;
    prev_best = detail::_best_move;

    if (!detail::_without_time) {
      // An answer that has not moved in several iterations is unlikely to move
      // now; one that just changed, or a score that fell, buys more time.
      double scale = 1.0 - STABILITY_STEP * stability;
      if (score_drop >= PANIC_SCORE_DROP)
        scale += PANIC_BONUS;

      const double budget = detail::_soft_limit * std::clamp(scale, SOFT_SCALE_MIN, SOFT_SCALE_MAX);

      // The soft limit governs the average spend across the game.
      if (elapsed >= budget)
        break;

      // The prediction only guards the hard limit. Opening an iteration that
      // gets killed is pure loss, because iter_deep publishes nothing from an
      // incomplete one, so its whole cost buys zero depth.
      const double ebf = prev_elapsed > 0 ? static_cast<double>(elapsed) / prev_elapsed : EBF_MAX;
      if (elapsed * std::clamp(ebf, EBF_MIN, EBF_MAX) > detail::_ms_allocated)
        break;

      prev_elapsed = elapsed;
    }
  }

  // A "go" command must always be answered; otherwise, the GUI will wait forever,
  // and making an illegal move results in an immediate forfeit.
  //
  // _best_move is null here for two different reasons. If the stop arrived before
  // depth 1 finished, moves[0] rescues it. If the position has no legal moves at
  // all it stays null, and the caller reports "0000" for the GUI to adjudicate.
  if (detail::_best_move.get_flag() == Move::NULL_MOVE) {
    Movegen movegen(board);
    MoveList &moves = movegen.get_legal_moves();

    if (moves.size() > 0)
      detail::_best_move = moves[0];
  }
}

int32_t Search::detail::_negamax(Board &board, int16_t depth, int32_t alpha, int32_t beta,
                                 bool null_move) {
  if (_check_limits())
    return 0;

  // Captured here because _order_info is incremented before the move loop, so
  // from inside that loop get_ply() already reads ply + 1.
  const int16_t ply = _order_info.get_ply();

  // Cleared before the depth check, not after it. A node that delegates straight
  // to _quiescence still owns a row, and leaving that row stale lets the parent
  // copy up a line left behind by whichever sibling subtree wrote it last;
  // which reads as a perfectly plausible PV with garbage on the end.
  _pv_length[ply] = ply;

  if (ply >= MAX_SEARCH_PLY - 1)
    return Eval::evaluate(board);

  if (depth < 1)
    return _quiescence(board, alpha, beta);

  ++_nodes;
  if (ply > _seldepth)
    _seldepth = ply;

  // A mate delivered on the fiftieth move stands; the draw claim never happens.
  // So the terminal test has to come first, and only a position with a legal
  // reply is a draw here.
  if (board.get_ply() >= MAX_PLY) {
    Movegen fifty_movegen(board);
    if (fifty_movegen.get_legal_moves().size() > 0)
      return 0;
    return board.king_in_check(board.get_curr_move()) ? -INF + ply : 0;
  }

  if (ply > 0 && board.is_repetition())
    return 0;

  ZobristHash zob_hash = board.get_zob_hash();

  // If this node is in the table, the entry answers two questions:
  // 1. Can this node be cut off outright?
  // 2. Which move should be tried first?
  const TTEntry *tt = TTable::probe(zob_hash);
  const Move tt_move = tt != nullptr ? tt->move : Move();

  if (tt != nullptr && ply > 0 && tt->depth >= depth) {
    const int32_t score = _score_from_tt(tt->score, ply);

    if ((tt->flag == TTFlag::EXACT) || (tt->flag == TTFlag::BETA && score >= beta) ||
        (tt->flag == TTFlag::ALPHA && score <= alpha))
      return score;
  }

  /// https://chessprogramming.org/Internal_Iterative_Reductions
  if (tt == nullptr && depth >= 4)
    --depth;

  bool in_check = board.king_in_check(board.get_curr_move());
  if (in_check && ply < 2 * _root_depth)
    ++depth;

  const int32_t static_eval = in_check ? 0 : Eval::evaluate(board);

  // Am I too far ahead to bother?
  // If my score much more than that I can have, so I do not bother to improve it.
  // We can cut off, because searching probably does not change the decision
  // Reverse futility. ply > 0 because we need some move.
  if (!in_check && ply > 0 && depth <= RFP_MAX_DEPTH && std::abs(beta) < MATE_BOUND &&
      static_eval - RFP_MARGIN * depth >= beta)
    return static_eval - RFP_MARGIN * depth;

  // Greatly speeds up the work. Approximately +150 Elo
  if (null_move && !in_check && ply > 0 && board.curr_player_has_big_pieces() &&
      depth >= NULL_MOVE_MIN_DEPTH) {
    // beta is INF whenever the root window is wide, so the subtraction has to be
    // guarded or it overflows.
    const int32_t surplus = std::abs(beta) < MATE_BOUND ? static_eval - beta : 0;
    const auto r =
        static_cast<int16_t>(NULL_MOVE_BASE_R + depth / NULL_MOVE_DEPTH_DIV +
                             std::clamp(surplus / NULL_MOVE_EVAL_DIV, 0, NULL_MOVE_MAX_R));

    board.make_null_move();
    ++_order_info;

    int32_t score = -_negamax(board, static_cast<int16_t>(depth - r), -beta, -beta + 1, false);

    --_order_info;
    board.unmake_null_move();

    if (_check_limits())
      return 0;

    // to prevent bug with mate
    if (score >= beta && std::abs(score) < MATE_BOUND)
      return score;
  }

  Movegen movegen(board);
  MoveList &move_list = movegen.get_legal_moves();

  // get size in O(1)
  // checkmate or stalemate
  if (move_list.size() == 0)
    return in_check ? -INF + _order_info.get_ply() : 0;


  // Captured before the loop: inside it the board is made, so get_curr_move()
  // would answer with the opponent.
  const Color us = board.get_curr_move();
  MovePicker move_picker = MovePicker(board, &move_list, tt_move, _order_info);

  Move curr_best_move = Move();
  int32_t curr_best_score = -INF;
  int32_t old_alpha = alpha;

  // Must be done before ++_order_info
  const Move killer1 = _order_info.get_killer1();
  const Move killer2 = _order_info.get_killer2();
  ++_order_info;

  // https://chessprogramming.org/Late_Move_Reductions
  int16_t move_count = 0;

  // Separate from move_count on purpose to debug info.
  int16_t searched = 0;

  while (move_picker.has_next()) {
    Move move = move_picker.get_next();
    ++move_count;

    // Futility. MovePicker sorts best-first, so if many moves we haven't good moves,
    // then assume next one does not improve the alpha.
    if (ply > 0 && !in_check && !move.is_tactical() && curr_best_score > -MATE_BOUND) {
      // Is this move too far behind to catch up?
      if (depth <= FUTILITY_MAX_DEPTH && static_eval + FUTILITY_MARGIN * depth <= alpha)
        continue;
    }

    ++searched;
    board.make(move);

    int32_t score;
    if (move_count == 1)
      score = -_negamax(board, depth - 1, -beta, -alpha, true);
    else {
      int16_t r = 0;

      // Never do it in the root.
      if (ply > 0 && depth >= 3 && move_count > 3 && !in_check && !move.is_tactical() &&
          !(move == killer1) && !(move == killer2)) {
        r = _lmr[std::min<int>(depth, MAX_SEARCH_DEPTH)][std::min<int>(move_count, 63)];

        // A quiet the history table already likes gets searched closer to full
        // depth. One-directional on purpose: nothing is reduced *more* yet, so a
        // failure here is the coupling, not a second new constant.
        const int32_t hist = _order_info.get_history(us, move.get_from_cell(), move.get_to_cell());
        r -= std::min<int16_t>(static_cast<int16_t>(hist / LMR_HISTORY_DIV), LMR_HISTORY_MAX);

        r = std::clamp<int16_t>(r, 0, depth - 2);
      }
      score = -_negamax(board, depth - 1 - r, -alpha - 1, -alpha, true);

      if (r > 0 && score > alpha)
        score = -_negamax(board, depth - 1, -alpha - 1, -alpha, true);
      if (score > alpha && score < beta)
        score = -_negamax(board, depth - 1, -beta, -alpha, true);
    }
    board.unmake(move);

    if (score > curr_best_score) {
      curr_best_score = score;
      curr_best_move = move;

      if (score > alpha) {
        if (score >= beta) {
          --_order_info;
          if (searched == 1)
            ++_fhf;
          ++_fh;

          if (!move.is_capture()) {
            _order_info.add_killer(move);
            _order_info.add_history(board.get_curr_move(), move.get_from_cell(), move.get_to_cell(),
                                    depth);
          }

          if (!_stop)
            TTable::add(zob_hash, curr_best_move, _score_to_tt(curr_best_score, ply), depth,
                        TTFlag::BETA);
          return curr_best_score;
        }
        alpha = score;

        if (ply + 1 < MAX_SEARCH_PLY) {
          _pv[ply][ply] = move;

          for (int16_t i = ply + 1; i < _pv_length[ply + 1]; ++i)
            _pv[ply][i] = _pv[ply + 1][i];

          _pv_length[ply] = _pv_length[ply + 1];
        }
      }
    }
  }
  --_order_info;

  if (!_stop) {
    if (alpha != old_alpha)
      TTable::add(zob_hash, curr_best_move, _score_to_tt(curr_best_score, ply), depth,
                  TTFlag::EXACT);
    else
      TTable::add(zob_hash, curr_best_move, _score_to_tt(curr_best_score, ply), depth,
                  TTFlag::ALPHA);
  }
  return curr_best_score;
}

int32_t Search::detail::_quiescence(Board &board, int32_t alpha, int32_t beta) {
  if (_check_limits())
    return 0;

  ++_nodes;
  const int16_t ply = _order_info.get_ply();
  if (ply >= MAX_SEARCH_PLY - 1)
    return Eval::evaluate(board);

  if (ply > _seldepth)
    _seldepth = ply;

  if (board.get_ply() >= MAX_PLY) {
    Movegen fifty_movegen(board);
    if (fifty_movegen.get_legal_moves().size() > 0)
      return 0;
    return board.king_in_check(board.get_curr_move()) ? -INF + ply : 0;
  }

  if (board.is_repetition())
    return 0;

  const bool in_check = board.king_in_check(board.get_curr_move());

  // https://www.chessprogramming.org/Quiescence_Search#Standing_Pat
  // Standing pat is the option to decline to move, which does not exist in
  // check, so a checked node starts from nothing and has to find a real evasion.
  int32_t stand_pat = 0;
  int32_t best_score = -INF;

  if (!in_check) {
    stand_pat = Eval::evaluate(board);
    best_score = stand_pat;

    if (best_score >= beta)
      return best_score;
    if (best_score > alpha)
      alpha = best_score;
  }

  Movegen movegen(board, Movegen::QUIESCENCE);
  MoveList &move_list = movegen.get_legal_moves();

  if (move_list.size() == 0)
    return in_check ? -INF + ply : best_score;

  // Quiescence never cuts on the table and never writes to it, so the entry is
  // only used to order the first move.
  const TTEntry *tt = TTable::probe(board.get_zob_hash());
  MovePicker move_picker =
      MovePicker(board, &move_list, tt != nullptr ? tt->move : Move(), _order_info);

  int16_t searched = 0;
  ++_order_info;

  while (move_picker.has_next()) {
    Move move = move_picker.get_next();

    // Neither prune is sound in check.
    if (!in_check) {
      int32_t gain = See::VALUE[move.get_captured_piece()];
      if (move.get_flag() == Move::CAPTURE_PROMOTION)
        gain += See::VALUE[move.get_promotion_piece()] - See::VALUE[PAWN];

      if (stand_pat + gain + DELTA_MARGIN <= alpha)
        continue;

      // A capture that loses material cannot be the move that makes this position quiet.
      if (See::can_lose_material(move) && See::see(board, move) < 0)
        continue;
    }

    ++searched;
    board.make(move);
    const int32_t score = -_quiescence(board, -beta, -alpha);
    board.unmake(move);

    if (score > best_score) {
      best_score = score;

      if (score > alpha) {
        if (score >= beta) {
          --_order_info;

          if (searched == 1)
            ++_q_fhf;
          ++_q_fh;

          return best_score;
        }
        alpha = score;
      }
    }
  }
  --_order_info;

  return best_score;
}