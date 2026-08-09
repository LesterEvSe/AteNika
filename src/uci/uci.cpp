#include "uci/uci.hpp"

#include <atomic>
#include <chrono>
#include <exception>
#include <iostream>
#include <print>
#include <sstream>
#include <string>
#include <thread>

#include "core/board.hpp"
#include "core/move.hpp"
#include "search/search.hpp"
#include "search/ttable.hpp"
#include "uci/extras.hpp"
#include "uci/output.hpp"

// Defined by CMake from project(VERSION).
#ifndef ATENIKA_VERSION
#define ATENIKA_VERSION "dev"
#endif

// Local scope
namespace {
  using Output::reply;

  constexpr const char *ENGINE_NAME = "AteNika " ATENIKA_VERSION;
  constexpr const char *ENGINE_AUTHOR = "LesterEvSe";

  std::atomic<bool> searching(false);
  std::atomic<bool> quitting(false);

  Board board;

  // searching is set by the caller, before the thread starts (not here).
  // Setting it here left a window where the main loop could see it false,
  // decide no search was running, and return out of Uci::start while this
  // thread was still touching `board`.
  void search_thread() {
    Search::iter_deep(board, true);

    if (!quitting) {
      const Move *best = Search::get_best_move();

      // "0000" is the conventional null move for a position with no legal reply.
      reply("bestmove {}", best ? static_cast<std::string>(*best) : std::string("0000"));
    }
    searching = false;
  }

  void handle_uci() {
    reply("id name {}", ENGINE_NAME);
    reply("id author {}", ENGINE_AUTHOR);

    // No options advertised yet. "Hash" belongs here, but the table is still an
    // unbounded std::unordered_map that would ignore whatever size we claimed
    // to support; it arrives with the rewrite in Phase 1.3.
    reply("uciok");
  }

  void handle_position(std::istringstream &args) {
    std::string token;
    if (!(args >> token))
      return;

    if (token == "startpos") {
      board = Board();
      args >> token; // "moves" or nothing

    } else if (token == "fen") {
      std::string fen;
      while (args >> token && token != "moves")
        fen += token + ' ';
      board = Board(fen);

    } else {
      std::println(std::cerr, "Unknown position type: {}", token);
      return;
    }

    if (token != "moves")
      return;

    // Replayed rather than jumped to: threefold_rule() walks the move history
    // that make() builds, so a position reconstructed from its FEN alone cannot
    // see a repetition the game actually contains.
    std::string notation;
    while (args >> notation) {
      try {
        const Move move(board, notation);
        board.make(move);
      } catch (const std::exception &e) {
        // Stop at the first bad move.
        std::println(std::cerr, R"(Rejected move "{}": {})", notation, e.what());
        return;
      }
    }
  }

  void handle_go(std::istringstream &args) {
    Search::Limits limits;
    std::string token;

    while (args >> token) {
      if (token == "wtime")
        args >> limits.time[WHITE];
      else if (token == "btime")
        args >> limits.time[BLACK];
      else if (token == "winc")
        args >> limits.inc[WHITE];
      else if (token == "binc")
        args >> limits.inc[BLACK];
      else if (token == "movestogo")
        args >> limits.movestogo;
      else if (token == "movetime")
        args >> limits.movetime;
      else if (token == "depth")
        args >> limits.depth;
      else if (token == "nodes")
        args >> limits.nodes;
      else if (token == "infinite")
        limits.infinite = true;

      // "ponder", "mate" and "searchmoves" fall through and are skipped.
      // Answering a "go" we only partly understood still beats not answering.
    }

    Search::set_limits(limits, board.get_curr_move());
    searching = true;
    std::thread(search_thread).detach();
  }
} // namespace

void Uci::start() {
  reply("{} by {}", ENGINE_NAME, ENGINE_AUTHOR);
  reply(R"("help" displays all commands)");

  board = Board();
  std::string input;

  // The loop condition is the fix for the spin: on closed stdin getline fails,
  // leaves input empty and keeps failing forever, so `while (true)` burned a
  // core printing "Incorrect command". Every match runner closes stdin.
  while (std::getline(std::cin, input)) {
    // Declared inside the loop on purpose. When it lived outside and the
    // line was empty, `args >> token` failed and left the previous command
    // in place. For example pressing Enter after "perft 4" re-entered the perft
    // branch with no argument.
    std::istringstream args(input);
    std::string token;
    args >> token;

    if (token.empty())
      continue;

    if (token == "isready") {
      reply("readyok");
      continue;
    }
    if (token == "stop") {
      Search::stop();
      continue;
    }
    if (token == "quit")
      break; // shutdown is handled once, after the loop

    // Everything below reads or replaces the board the search thread is using.
    if (searching) {
      std::println(std::cerr, "Busy searching, ignored: {}", input);
      continue;
    }

    if (token == "uci") {
      handle_uci();

    } else if (token == "debug") {
      std::string value;
      args >> value;
      Search::set_debug(value == "on");

    } else if (token == "ucinewgame") {
      TTable::clear();
      board = Board();

    } else if (token == "position") {
      handle_position(args);

    } else if (token == "go") {
      handle_go(args);

    } else if (token == "setoption") {
      // Accepted and ignored. We advertise no options, but GUIs send these
      // unprompted and one that gets an error back may refuse to start us.

    } else if (!Extras::dispatch(board, token, args)) {
      std::println(std::cerr, "Unknown command: {}", input);
    }
  }

  // Reached by "quit" or by stdin closing. Either way a detached search thread
  // may still be running, so ask it to stop and wait for it. Sleeping rather
  // than spinning: the old `while (lock);` busy-waited at 100% CPU.
  quitting = true;
  while (searching) {
    // Re-asserted on every pass: iter_deep opens with _restart(), which clears
    // the stop flag, so a single stop racing ahead of it would be swallowed.
    Search::stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
