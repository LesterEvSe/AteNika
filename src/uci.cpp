#include "uci.hpp"
#include <sstream>
#include <functional>
#include <thread>
#include <atomic>

namespace {
    std::atomic<bool> lock(false);
    std::atomic<bool> quit(false);

    void go(Board &board, bool debug) {
        lock = true;
        Search::iter_deep(board, debug);
        board.make(Search::get_best_move());

        if (!quit)
            std::cout << "\nEngine's move: " << Search::get_best_move();
        lock = false;
    }
}

void Uci::init(std::string book_path) {
    if (book_path.empty()) return;
}

void Uci::start() {
    std::cout << "AteNica by LesterEvSe" << std::endl;
    std::cout << "\"help\" displays all commands" << std::endl;

    static constexpr size_t n = std::numeric_limits<std::streamsize>::max();
    Board board;
    Search::restart();
    std::string input, command;

    while(1) {
        std::cout << "AteNica> ";
        std::getline(std::cin, input); // check how it works
        std::istringstream iss(input);
        iss >> command;

        if (command == "go" || command == "godeb") {
            if (lock) { std::cout << "This command is not available now" << std::endl; continue; }
            std::thread search([&board, &command]{ return go(board, command == "godeb"); });
            search.detach();
        } else if (command == "newgame") {
            if (lock) { std::cout << "This command is not available now" << std::endl; continue; }
            board = Board();
            Search::restart();
        } else if (command == "prb") {
            if (lock) { std::cout << "This command is not available now" << std::endl; continue; }
            std::cout << board;
        } else if (command == "stop") {
            Search::stop();
        } else if (command == "quit") {
            quit = true;
            Search::stop();
            break;
        } else if (command == "help") {
            std::cout << "go - find and print best move" << std::endl;
            std::cout << "godeb - go with debug information" << std::endl;
            std::cout << "newgame - start new game" << std::endl;
            std::cout << "prb - print board" << std::endl;
            std::cout << "stop - Instantly stops the search and returns last best move" << std::endl;
            std::cout << "quit - exit the program" << std::endl;
        } else
            std::cout << "Incorrect command, try again" << std::endl;
    }
    
    while (lock); // waiting for finished the thread
    std::cout << "See you later!" << std::endl;
}