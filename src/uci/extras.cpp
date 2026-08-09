#include "uci/extras.hpp"

#include <chrono>
#include <exception>
#include <iostream>

#include "core/board.hpp"
#include "core/move.hpp"
#include "eval/eval.hpp"
#include "movegen/perft.hpp"
#include "search/bench.hpp"
#include "search/search.hpp"
#include "uci/output.hpp"

namespace {
  using Output::reply;

  // Shared by bench and perft. Returns false and reports usage on anything that
  // is not a whole positive number.
  bool parse_depth(const std::string &arg, int &out, const char *usage) {
    try {
      size_t pos;
      const int value = std::stoi(arg, &pos);

      if (pos != arg.size() || value < 1)
        throw std::runtime_error("");

      out = value;
      return true;

    } catch (const std::exception &) {
      // std::cerr has unitbuf set by the standard, so it flushes itself.
      std::println(std::cerr, "{}", usage);
      return false;
    }
  }

  void run_bench(std::istringstream &args) {
    // Optional depth override: "bench" or "bench 10".
    std::string arg;
    int depth = Bench::DEFAULT_DEPTH;

    if ((args >> arg) && !parse_depth(arg, depth, "Usage: bench [depth]"))
      return;

    Bench::run(depth);
  }

  void run_perft(Board &board, std::istringstream &args) {
    std::string arg;
    args >> arg;
    const bool divide = (arg == "divide");
    if (divide)
      args >> arg;

    int depth;
    if (!parse_depth(arg, depth, "Usage: perft <depth> | perft divide <depth>"))
      return;

    const auto start = std::chrono::steady_clock::now();
    const int64_t nodes = divide ? Perft::divide(board, depth) : Perft::run(board, depth);
    const int64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::steady_clock::now() - start)
                           .count();

    if (!divide)
      reply("Nodes searched: {}", nodes);
    reply("Time: {} ms  ({} knps)", ms, nodes / (ms + 1));
  }

  // Bare coordinate notation typed at the prompt: "e2e4", or a whole line of
  // them.
  void play_moves(Board &board, const std::string &first, std::istringstream &args) {
    // All or nothing strategy. Without the snapshot a bad move halfway through a line
    // leaves the position silently advanced by however many moves did parse,
    // and the early return skips the board dump that would have shown it.
    const Board snapshot = board;

    std::string notation = first;
    do {
      if (!Move::isMove(notation)) {
        std::println(std::cerr, "Not a move: {}", notation);
        board = snapshot;
        return;
      }

      try {
        const Move move(board, notation);
        board.make(move);

      } catch (const std::exception &e) {
        std::println(std::cerr, R"(Rejected move "{}": {})", notation, e.what());
        board = snapshot;
        return;
      }
    } while (args >> notation);

    if (board.threefold_rule())
      reply("Draw by threefold repetition");
    std::cout << board << std::flush;
  }
} // namespace

bool Extras::dispatch(Board &board, const std::string &token, std::istringstream &args) {
  if (token == "d") {
    std::cout << board << std::flush;

  } else if (token == "state") {
    board.display_all();
    reply("Search time : {}", Search::get_allocated_sec());
    reply("Search depth: {} plies", Search::get_search_depth());

  } else if (token == "eval") {
    reply("Static evaluation: {} cp", Eval::evaluate(board));

  } else if (token == "bench") {
    run_bench(args);

  } else if (token == "perft") {
    run_perft(board, args);

  } else if (token == "help") {
    help();

  } else if (Move::isMove(token)) {
    play_moves(board, token, args);

  } else {
    return false;
  }
  return true;
}

void Extras::help() {
  reply("UCI commands:");
  reply("  uci                       identify the engine and list its options");
  reply("  isready                   answered with \"readyok\", also during a search");
  reply("  ucinewgame                forget everything learned from the previous game");
  reply("  position startpos [moves ...]");
  reply("  position fen <fen> [moves ...]");
  reply("  go [depth n] [nodes n] [movetime ms] [infinite]");
  reply("  go [wtime ms] [btime ms] [winc ms] [binc ms] [movestogo n]");
  reply("  stop                      end the search now and print the best move so far");
  reply("  setoption name <id> value <x>");
  reply("  quit\n");

  reply("Non-standard commands:");
  reply("  d                         display the current position");
  reply("  state                     verbose board dump plus the current search limits");
  reply("  eval                      static evaluation of the current position");
  reply("  bench [n]                 fixed position set at fixed depth; the node count");
  reply("                            fingerprints the search");
  reply("  perft n                   count legal move paths of depth n");
  reply("  perft divide n            same, per root move (diff against Stockfish's");
  reply("                            \"go perft n\")");
  reply("  help                      this text\n");

  reply("Coordinate notation on its own line plays the move(s) on the current");
  reply("position, e.g. \"e2e4\" or \"e2e4 e7e5 g1f3\". Add the piece letter for a");
  reply("promotion: q, r, b or n, as in \"a7a8q\".\n");
}
