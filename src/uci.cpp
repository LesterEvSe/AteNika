#include "uci.hpp"
#include <sstream>
#include <thread>
#include <atomic>
#include <exception>

namespace {
    std::atomic<bool> lock(false);
    std::atomic<bool> quit(false);

    Board board;
    History history;

    void go(bool debug) {
        lock = true;
        Search::iter_deep(history, board, debug);

        if (quit) {
            lock = false;
            return;
        }

        if (!Search::get_best_move())
            std::cout << "\nEngine's move: No moves" << std::endl;
        else
            // std::endl required, otherwise,
            // it will be output only after entering the next command
            std::cout << "\nEngine's move: " << static_cast<std::string>(*Search::get_best_move()) << std::endl;
        lock = false;
    }
}

void Uci::init(std::string book_path) {
    if (book_path.empty()) return;
}

void Uci::start() {
    std::cout << "AteNica by LesterEvSe\n\n";
    std::cout << "\"help\" displays all commands" << std::endl << std::endl;

    board = Board();
    history = History();
    std::string input, command;

    while(1) {
        std::cout << "AteNica> ";
        std::getline(std::cin, input); // check how it works
        std::istringstream iss(input);
        iss >> command;

        if (input == "go" || input == "godeb") {
            if (lock) { std::cout << "This command is not available now" << std::endl; continue; }
            std::thread search([&command]{ return go(command == "godeb"); });
            search.detach();

        } else if (input == "ucinewgame") {
            if (lock) { std::cout << "This command is not available now" << std::endl; continue; }
            board = Board();
            history.clear();

        } else if (command == "depth" || command == "time") {
            if (lock) { std::cout << "This command is not available now" << std::endl; continue; }
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
                if (pos != value.size()) throw std::exception();

            } catch (const std::exception &e) {
                std::cout << "Incorrect command, try again" << std::endl;
                continue;
            }

            if (command == "depth") {
                Search::set_depth(number);
                Search::set_time(INF);
            } else
                Search::set_time(number * 1000);

        } else if (input == "d") {
            if (lock) { std::cout << "This command is not available now" << std::endl; continue; }
            board.display_all();
        } else if (input == "stop") {
            if (!lock) { std::cout << "No search is performed" << std::endl; continue; }
            Search::stop();
        } else if (input == "quit") {
            if (lock) {
                quit = true;
                Search::stop();
            }
            break;
        } else if (input == "help") {
            std::cout << "go - find and print best move" << std::endl;
            std::cout << "godeb - \"go\" command with debug information" << std::endl;
            std::cout << "ucinewgame - start new game" << std::endl;
            std::cout << "depth n - search for \"n\" nodes in depth" << std::endl;
            std::cout << R"(time n - search for "n" seconds per move or "inf" to disregard time)" << std::endl;
            std::cout << "d - display the current position" << std::endl;
            std::cout << "stop - Instantly stops the search and returns last best move" << std::endl;
            std::cout << "quit - exit the program" << std::endl;
            std::cout << "Enter move in coordinate notation, e.g., e4e5, c4e6." << std::endl <<
                         "Or for promotion piece add last symbol q (queen), n (knight), b (bishop) or r (rook) a7a8q" << std::endl;
        } else if (Move::isMove(input)) {
            if (lock) { std::cout << "This command is not available now" << std::endl; continue; }

            Move move;
            try {
                move = Move(board, command);
            } catch (std::exception &e) {
                std::cout << e.what() << std::endl;
                continue;
            }

            board.make(move);
            if (board.get_ply() == 0)
                history.clear();
            else
                history.add_pos(board.get_zob_hash());
        } else
            std::cout << "Incorrect command, try again" << std::endl;
    }
    
    while (lock); // waiting for finished the thread
    std::cout << "See you later!" << std::endl;
}