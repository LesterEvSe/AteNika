#include "uci/interface.hpp"

#include <chrono>
#include <iostream>
#include <print>
#include <sstream>
#include <thread>
#include <utility>

#include "core/board.hpp"
#include "eval/eval.hpp"
#include "movegen/perft.hpp"
#include "search/bench.hpp"
#include "search/search.hpp"

// Local scope
namespace {
  std::atomic<bool> lock(false);
  std::atomic<bool> quit(false);

  Board board;

  // std::cout is line-buffered only on a terminal; on a pipe — which is how a
  // GUI, fastchess and `echo go | AteNika` all run the engine — it is fully
  // buffered, and std::println does not flush. Without the flush below, output
  // from the detached search thread appears only after the next command.
  template <class... Args> void reply(std::format_string<Args...> fmt, Args &&...args) {
    std::println(std::cout, fmt, std::forward<Args>(args)...);
    std::cout.flush();
  }

  // lock is set by the caller, before the thread starts (not here). Setting it
  // here left a window where the main loop could see lock == false, decide no
  // search was running, and return out of Uci::start while this thread was
  // still touching `board`.
  void go(bool debug) {
    Search::iter_deep(board, debug);

    if (quit) {
      lock = false;
      return;
    }

    Move *move = Search::get_best_move();
    reply("\nEngine's move: {}", move ? static_cast<std::string>(*move) : std::string("no moves"));
    lock = false;
  }
} // namespace

void Uci::start() {
  reply("\nAteNika by LesterEvSe");
  reply("\"help\" displays all commands\n");

  board = Board();
  std::string input;

  auto check_lock = []() {
    if (!lock)
      return false;
    reply("This command is not available now");
    return true;
  };

  // The loop condition is the fix for the spin: on closed stdin getline fails,
  // leaves input empty and keeps failing forever, so `while (true)` burned a
  // core printing "Incorrect command". Every match runner closes stdin.
  while (std::getline(std::cin, input)) {
    std::istringstream iss(input);

    // Declared inside the loop on purpose. When it lived outside and the
    // line was empty, `iss >> command` failed and left the previous
    // command in place — pressing Enter after "perft 4" re-entered the perft
    // branch with no argument.
    std::string command;
    iss >> command;

    if (command.empty())
      continue;

    if (input == "go" || input == "godeb") {
      if (lock) {
        reply("This command is not available now");
        continue;
      }
      lock = true;
      std::thread search([command] { go(command == "godeb"); });
      search.detach();

    } else if (input == "newgame") {
      if (check_lock())
        continue;
      board = Board();

    } else if (command == "setfen") {
      if (check_lock())
        continue;
      std::string fenstr;
      std::getline(iss, fenstr);
      board = Board(fenstr);

    } else if (command == "depth" || command == "time") {
      if (check_lock())
        continue;
      std::string value;
      std::getline(iss, value);

      if (command == "time" && value.substr(1) == "inf") {
        Search::set_time(INF);
        continue;
      }

      size_t pos;
      int16_t number;
      try {
        number = static_cast<int16_t>(std::stoi(value, &pos));
        if (pos != value.size())
          throw std::runtime_error("");

      } catch (const std::exception &) {
        // std::cerr has unitbuf set by the standard, so it flushes itself.
        std::println(std::cerr, "Incorrect command, try again");
        continue;
      }

      if (command == "depth")
        Search::set_depth(number);
      else
        Search::set_time(number * 1000);

    } else if (input == "d" || input == "info") {
      if (check_lock())
        continue;
      if (input == "d")
        std::cout << board << std::flush;
      else {
        board.display_all();
        reply("Search Time: {}", Search::get_allocated_sec());
        reply("Search Depth: {} plies", Search::get_search_depth());
      }

    } else if (command == "bench") {
      if (check_lock())
        continue;

      // Optional depth override: "bench" or "bench 10"
      std::string arg;
      int depth = Bench::DEFAULT_DEPTH;
      if (iss >> arg) {
        try {
          size_t pos;
          depth = std::stoi(arg, &pos);
          if (pos != arg.size() || depth < 1)
            throw std::runtime_error("");
        } catch (const std::exception &) {
          std::println(std::cerr, "Usage: bench [depth]");
          continue;
        }
      }
      Bench::run(depth);

    } else if (command == "perft") {
      if (check_lock())
        continue;

      // "perft n" or "perft divide n"
      std::string arg;
      iss >> arg;
      bool div = (arg == "divide");
      if (div)
        iss >> arg;

      int depth;
      try {
        size_t pos;
        depth = std::stoi(arg, &pos);
        if (pos != arg.size() || depth < 1)
          throw std::runtime_error("");
      } catch (const std::exception &) {
        std::println(std::cerr, "Usage: perft <depth> | perft divide <depth>");
        continue;
      }

      auto start = std::chrono::steady_clock::now();
      int64_t nodes = div ? Perft::divide(board, depth) : Perft::run(board, depth);
      int64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::steady_clock::now() - start)
                       .count();

      if (!div)
        reply("Nodes searched: {}", nodes);
      reply("Time: {} ms  ({} knps)", ms, nodes / (ms + 1));

    } else if (input == "eval") {
      if (check_lock())
        continue;
      reply("Static Evaluation: {} cp", Eval::evaluate(board));

    } else if (input == "stop") {
      if (!lock) {
        reply("No search is performed");
        continue;
      }
      Search::stop();
    } else if (input == "quit") {
      break; // shutdown is handled once, after the loop
    } else if (input == "help") {
      help();

    } else if (Move::isMove(input)) {
      if (check_lock())
        continue;
      if (board.threefold_rule())
        reply("Tie, repeating the position 3 times");
      Move move;
      try {
        move = Move(board, command);
        board.make(move);
      } catch (std::exception &e) {
        reply("{}", e.what());
      }

    } else
      reply("Incorrect command. Type \"help\" for more information.");
  }

  // Reached by "quit" or by stdin closing. Either way a detached search thread
  // may still be running, so ask it to stop and wait for it. Sleeping rather
  // than spinning: the old `while (lock);` busy-waited at 100% CPU.
  quit = true;
  while (lock) {
    Search::stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void Uci::help() {
  reply("go - find and print best move");
  reply("godeb - \"go\" command with debug information");
  reply("newgame - start new game");
  reply("setfen fen_str - reset up the position described in fenstring (assuming "
        "the string is correct)");
  reply("depth n - search for \"n\" plies deep");
  reply(R"(time n - search for "n" seconds per move or "inf" to disregard time)");
  reply("d - display the current position");
  reply("info - display information about search and more precise about board");
  reply("eval - static evaluation of current position");
  reply("bench [n] - fixed position set at fixed depth; the node count "
        "fingerprints the search");
  reply("perft n - count all legal move paths of depth \"n\" from the current position");
  reply("perft divide n - same, broken down per root move (diff against "
        "Stockfish's \"go perft n\")");
  reply("stop - Instantly stops the search and returns last best move");
  reply("quit - exit the program\n");

  reply("Enter move in coordinate notation, e.g., e4e5, c4e6.");
  reply("Or for promotion piece add_and_inc last symbol q (queen), n (knight), b "
        "(bishop) or r (rook).");
  reply("E.g., a7a8q, d2d1r");
}