#include "search/search.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <format>
#include <iostream>
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
  bool _debug_info;
  std::chrono::time_point<std::chrono::steady_clock> _start;

  std::string _mate; // for mate check

  // Declared ahead of the definitions below: iter_deep calls into them before
  // they appear, and _negamax and _quiescence are mutually recursive.
  void _info(const Board &board, int depth, int elapsed);
  void _restart();
  bool _check_limits();

  int32_t _negamax(Board &board, int16_t depth, int32_t alpha, int32_t beta, bool null_move);
  int32_t _quiescence(Board &board, int32_t alpha, int32_t beta);
} // namespace Search::detail

void Search::detail::_restart() {
  _nodes = 0;
  _stop = false;

  _fh = 0;
  _fhf = 0;
  _mate = "";
  _seldepth = 0;

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


void Search::detail::_info(const Board &board, int depth, int elapsed) {
  // Sometimes we have an error in Linux.
  // Process finished with exit code 136 (interrupted by signal 8:SIGFPE)
  // It's divide by zero error, so I increment elapsed ms, to avoid this problem
  const int64_t nps = _nodes * 1000 / (elapsed + 1);

  // The principal variation is still walked out of the transposition table.
  // That is only sound while the table is an unbounded map keyed on the whole
  // 96-bit hash, where an entry can be neither displaced nor aliased. Phase 1.3
  // replaces it with a fixed-size array, at which point a clobbered entry turns
  // this into a truncated — or illegal — line. The triangular PV table is the
  // fix, and it has to land before that rewrite.
  std::string pv;
  Board temp = board;

  // Set a counter, so we don't go over the limit
  for (int i = 0; i < _depth && TTable::in_table(temp.get_zob_hash()); ++i) {
    const Move move = TTable::get(temp.get_zob_hash()).move;

    // TTable::get is operator[], so a probe that misses inserts a default entry
    // whose move is NULL_MOVE. Making it would corrupt the scratch board.
    if (move.get_flag() == Move::NULL_MOVE)
      break;

    pv += static_cast<std::string>(move) + ' ';
    temp.make(move);
  }
  if (!pv.empty())
    pv.pop_back();

  std::println(std::cout, "info depth {} seldepth {} score {} nodes {} nps {} time {} pv {}", depth,
               _seldepth, format_score(_best_score), _nodes, nps, elapsed, pv);

  // moq — move ordering quality, the share of fail-highs that resolved on the
  // first move. Phase 2 reads it to judge whether staged generation is worth
  // the work, so it survives the move to UCI on the "info string" channel.
  if (_debug_info)
    std::println(std::cout, "info string moq {}% fh {} fhf {}",
                 static_cast<int>(100.0 * static_cast<double>(_fhf) /
                                  static_cast<double>(_fh == 0 ? 1 : _fh)),
                 _fh, _fhf);

  std::cout.flush();
}

void Search::iter_deep(Board &board, bool print_info) {
  detail::_restart();
  detail::_start = std::chrono::steady_clock::now();

  for (int16_t i = 1; i <= detail::_depth; ++i) {
    detail::_best_score = detail::_negamax(board, i, -INF, INF, true);

    // static_cast for MSVC W4 warnings
    auto elapsed = static_cast<int32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                                            std::chrono::steady_clock::now() - detail::_start)
                                            .count());

    if (detail::_stop)
      break;

    // Assigned before the info line so the PV printed and the move eventually
    // played are read from the same table state.
    detail::_best_move = TTable::get(board.get_zob_hash()).move;
    if (print_info)
      detail::_info(board, i, elapsed);

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
    MoveList moves = Movegen(board).get_legal_moves();

    if (moves.size() > 0)
      detail::_best_move = moves[0];
  }
}

int32_t Search::detail::_negamax(Board &board, int16_t depth, int32_t alpha, int32_t beta,
                                 bool null_move) {
  if (_check_limits())
    return 0;
  if (depth < 1)
    return _quiescence(board, alpha, beta);

  ++_nodes;
  if (_order_info.get_ply() > _seldepth)
    _seldepth = _order_info.get_ply();

  if (board.get_ply() >= MAX_PLY || board.threefold_rule())
    return 0;

  ZobristHash zob_hash = board.get_zob_hash();

  bool in_check = board.king_in_check(board.get_curr_move());
  if (in_check)
    ++depth;

  // Greatly speeds up the work. Should be +100 Elo
  if (null_move && !in_check && board.get_ply() && board.curr_player_has_big_pieces() &&
      depth >= 4) {
    board.make_null_move();
    int32_t score = _negamax(board, depth - 4, -alpha - 1, -alpha, false);
    board.unmake_null_move();

    if (_check_limits())
      return 0;

    // to prevent bug with mate
    if (score >= beta && std::abs(score) < 2'000'000'000)
      return beta;
  }

  MoveList move_list = Movegen(board).get_legal_moves();

  // get size in O(1)
  // checkmate or stalemate
  if (move_list.size() == 0)
    return in_check ? -INF + _order_info.get_ply() : 0;


  MovePicker move_picker = MovePicker(&move_list, zob_hash, _order_info);

  Move curr_best_move = Move();
  int32_t curr_best_score = -INF;
  int32_t old_alpha = alpha;

  // PVS - Principal Variation Search
  // https://www.chessprogramming.org/Principal_Variation_Search
  bool full_window = true;
  bool first_move = true;
  ++_order_info;

  while (move_picker.has_next()) {
    Move move = move_picker.get_next();
    board.make(move);

    int32_t score;
    if (full_window)
      score = -_negamax(board, depth - 1, -beta, -alpha, true);
    else {
      score = -_negamax(board, depth - 1, -alpha - 1, -alpha, true);
      if (score > alpha)
        score = -_negamax(board, depth - 1, -beta, -alpha, true);
    }
    board.unmake(move);

    if (score > curr_best_score) {
      curr_best_score = score;
      curr_best_move = move;

      if (score > alpha) {
        if (score >= beta) {
          --_order_info;
          if (first_move)
            ++_fhf;
          ++_fh;

          if (!(move.get_flag() & Move::CAPTURE))
            _order_info.add_killer(curr_best_move);

          TTable::add(zob_hash, {curr_best_move, curr_best_score, depth, BETA});
          return beta;
        }
        alpha = score;
        full_window = false;

        if (!(move.get_flag() & Move::CAPTURE))
          _order_info.add_history(curr_best_move.get_from_cell(), curr_best_move.get_to_cell(),
                                  depth);
      }
    }
    first_move = false;
  }
  --_order_info;

  // improve alpha
  if (alpha != old_alpha)
    TTable::add(zob_hash, {curr_best_move, curr_best_score, depth, EXACT});
  else
    TTable::add(zob_hash, {curr_best_move, alpha, depth, ALPHA});
  return alpha;
}

int32_t Search::detail::_quiescence(Board &board, int32_t alpha, int32_t beta) {
  // check limits here
  if (_check_limits())
    return 0;

  ++_nodes;
  if (_order_info.get_ply() > _seldepth)
    _seldepth = _order_info.get_ply();

  if (board.get_ply() >= MAX_PLY || board.threefold_rule())
    return 0;

  // https://www.chessprogramming.org/Quiescence_Search#Standing_Pat
  int32_t stand_pat = Eval::evaluate(board);
  if (stand_pat >= beta)
    return beta;
  if (stand_pat > alpha)
    alpha = stand_pat;

  MoveList move_list = Movegen(board).get_legal_moves();
  if (move_list.size() == 0)
    return board.king_in_check(board.get_curr_move()) ? -INF + _order_info.get_ply() : 0;

  ZobristHash zob_hash = board.get_zob_hash();
  QMovePicker q_move_picker = QMovePicker(&move_list, zob_hash);

  Move curr_best = Move();
  bool first_move = true;
  ++_order_info;

  while (q_move_picker.has_next()) {
    Move move = q_move_picker.get_next();
    board.make(move);
    stand_pat = -_quiescence(board, -beta, -alpha);
    board.unmake(move);

    if (stand_pat > alpha) {
      if (stand_pat >= beta) {
        --_order_info;
        if (first_move)
          ++_fhf;
        ++_fh;
        return beta;
      }
      alpha = stand_pat;
      curr_best = move;
    }
    first_move = false;
  }
  --_order_info;

  return alpha;
}