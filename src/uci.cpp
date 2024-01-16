#include "uci.hpp"

namespace {
    void go(Board &board, bool debug) {
        Search::iter_deep(board, debug);
        board.make(Search::get_best_move());
        std::cout << "Engine's move: " << Search::get_best_move();
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
    std::string command;

    while(1) {
        std::cout << "AteNica> ";
        std::getline(std::cin, command); // check how it work

        if (command == "go")
            go(board, false);
        else if (command == "godeb")
            go(board, true);
        else if (command == "search")
            ; // Search::set_time(); // n * 1000 in ms
        else if (command == "newgame") {
            board = Board();
            Search::restart();
        }
        else if (command == "prb")
            std::cout << board;
        else if (command == "quit")
            break;
        else if (command == "help") {
            std::cout << "go - find and print best move" << std::endl;
            std::cout << "godeb - go with debug information" << std::endl;
            std::cout << "search n - search for n seconds per move" << std::endl;
            std::cout << "newgame - start new game" << std::endl;
            std::cout << "prb - print board" << std::endl;
            std::cout << "quit - exit the program" << std::endl;
        } else
            std::cout << "Incorrect command, try again" << std::endl;
    }
    std::cout << "See you later!";
}