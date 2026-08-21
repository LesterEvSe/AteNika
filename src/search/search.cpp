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
#include "search/qmovepicker.hpp"
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

  // https://www.chessprogramming.org/Futility_Pruning#Move_Count_Based_Pruning
  // Late move pruning, 3 + depth * depth. Depth 1 is skipped on purpose: measured
  // first-move cutoff rate there is 64-89%, against 90%+ from depth 2 on, and it
  // carries more fail-highs than every other depth combined.
  constexpr int16_t LMP_MIN_DEPTH = 2;
  constexpr int16_t LMP_MAX_DEPTH = 4;

  // These two prove itself as +118 +/- 35 with sprt.
  // https://www.chessprogramming.org/Reverse_Futility_Pruning or static null move pruning.
  constexpr int16_t RFP_MAX_DEPTH = 6;
  constexpr int32_t RFP_MARGIN = 80;

  // https://www.chessprogramming.org/Futility_Pruning
  constexpr int16_t FUTILITY_MAX_DEPTH = 3;
  constexpr int32_t FUTILITY_MARGIN = 100;

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
  int32_t _ms_allocated;
  bool _without_time;
  int16_t _depth;
  int64_t _max_nodes;
  std::atomic<bool> _stop;

  int64_t _fh;  // cut-off at n move. The moves are accumulating.
  int64_t _fhf; // cut-off at first move.

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
  _mate = "";
  _seldepth = 0;
  _root_depth = 0;
  _best_score = 0;
  _best_pv_length = 0;

  _order_info = OrderInfo();
  _best_move = Move();
}

void Search::init() {
  detail::_restart();
  detail::_ms_allocated = 5000;
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
    detail::_ms_allocated = limits.movetime;
    return;
  }

  // Classic budget: an even slice of the remaining clock plus half the
  // increment. iter_deep already treats half of _ms_allocated as a soft limit
  // and will not open an iteration it cannot plausibly finish, so the real
  // spend lands between _ms_allocated / 2 and _ms_allocated.
  const int32_t remaining = limits.time[side_to_move];
  const int32_t moves_left = limits.movestogo > 0 ? std::min<int32_t>(limits.movestogo, 30) : 20;
  int32_t budget = remaining / moves_left + limits.inc[side_to_move] / 2;

  // Never plan to spend the whole clock. Pipe latency, process scheduling and
  // the 2048-node granularity of _check_limits all overshoot a little, and
  // flagging loses the game outright no matter how good the move was.
  budget = std::min(budget, remaining - MOVE_OVERHEAD_MS);
  detail::_ms_allocated = std::max(budget, 1);
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
  // first move. Phase 2 reads it to judge whether staged generation is worth
  // the work, so it survives the move to UCI on the "info string" channel.
  if (_debug_info)
    std::println("info string moq {}% fh {} fhf {}",
                 static_cast<int>(100.0 * static_cast<double>(_fhf) /
                                  static_cast<double>(_fh == 0 ? 1 : _fh)),
                 _fh, _fhf);

  std::fflush(stdout);
}

void Search::iter_deep(Board &board, bool print_info) {
  detail::_restart();
  detail::_start = std::chrono::steady_clock::now();

  TTable::new_search();

  int32_t prev_score = 0;

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

    if (!detail::_without_time && elapsed >= (detail::_ms_allocated / 2))
      break;
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
  if (null_move && !in_check && ply > 0 && board.curr_player_has_big_pieces() && depth >= 4) {
    board.make_null_move();
    ++_order_info;

    int32_t score = -_negamax(board, depth - 4, -beta, -beta + 1, false);

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


  MovePicker move_picker = MovePicker(&move_list, tt_move, _order_info);

  Move curr_best_move = Move();
  int32_t curr_best_score = -INF;
  int32_t old_alpha = alpha;

  // Must be done before ++_order_info
  const Move killer1 = _order_info.get_killer1();
  const Move killer2 = _order_info.get_killer2();
  ++_order_info;

  // https://chessprogramming.org/Late_Move_Reductions
  int16_t move_count = 0;
  int16_t quiet_count = 0;

  while (move_picker.has_next()) {
    Move move = move_picker.get_next();
    ++move_count;
    if (!move.is_tactical())
      ++quiet_count;

    // Late move pruning, then futility. Stops pruning if every move leads to defeat.
    // MovePicker sorts best-first, so if many moves we haven't good moves,
    // then assume next one does not improve the alpha.
    if (ply > 0 && !in_check && !move.is_tactical() && curr_best_score > -MATE_BOUND) {
      // Have enough quiet moves already failed? Non-PV only.
      if (beta == alpha + 1 && depth >= LMP_MIN_DEPTH && depth <= LMP_MAX_DEPTH &&
          quiet_count >= 3 + depth * depth)
        continue;

      // Is this move too far behind to catch up?
      if (depth <= FUTILITY_MAX_DEPTH && static_eval + FUTILITY_MARGIN * depth <= alpha)
        continue;
    }

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
          if (move_count == 1)
            ++_fhf;
          ++_fh;

          if (!move.is_capture()) {
            _order_info.add_killer(move);
            _order_info.add_history(move.get_from_cell(), move.get_to_cell(), depth);
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
  // check limits here
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

  // https://www.chessprogramming.org/Quiescence_Search#Standing_Pat
  int32_t best_score = Eval::evaluate(board);
  if (best_score >= beta)
    return best_score;
  if (best_score > alpha)
    alpha = best_score;

  Movegen movegen(board);
  MoveList &move_list = movegen.get_legal_moves();
  if (move_list.size() == 0)
    return board.king_in_check(board.get_curr_move()) ? -INF + _order_info.get_ply() : 0;

  // Quiescence never cuts on the table and never writes to it, so the entry is
  // only used to order the first move.
  const TTEntry *tt = TTable::probe(board.get_zob_hash());
  QMovePicker q_move_picker = QMovePicker(&move_list, tt != nullptr ? tt->move : Move());

  bool first_move = true;
  ++_order_info;

  while (q_move_picker.has_next()) {
    Move move = q_move_picker.get_next();
    board.make(move);
    const int32_t score = -_quiescence(board, -beta, -alpha);
    board.unmake(move);

    if (score > best_score) {
      best_score = score;

      if (score > alpha) {
        if (score >= beta) {
          --_order_info;
          if (first_move)
            ++_fhf;
          ++_fh;
          return best_score;
        }
        alpha = score;
      }
    }
    first_move = false;
  }
  --_order_info;

  return best_score;
}