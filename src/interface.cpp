#include "interface.hpp"
#include "board.hpp"
#include "search.hpp"

#include <sstream>
#include <thread>

// Local scope
namespace
{
    std::atomic<bool> lock(false);
    std::atomic<bool> quit(false);

    Board board;

    void go(bool debug) {
        lock = true;
        Search::iter_deep(board, debug);

        if (quit) {
            lock = false;
            return;
        }

        std::cout << "\nEngine's move: ";
        if (!Search::get_best_move())
            std::cout << "no moves" << std::endl;
        else
            // std::endl required, otherwise,
            // it will be output only after entering the next command
            std::cout << static_cast<std::string>(*Search::get_best_move()) << std::endl;
        lock = false;
    }
}

void Uci::start()
{
    std::cout << "\nAteNika by LesterEvSe\n";
    std::cout << "\"help\" displays all commands" << std::endl << std::endl;

    board = Board();
    std::string input, command;

    while (true) {
        std::getline(std::cin, input);
        std::istringstream iss(input);
        iss >> command;

        if (input == "go" || input == "godeb") {
            if (lock) { std::cout << "This command is not available now" << std::endl; continue; }
            std::thread search([command] { go(command == "godeb"); });
            search.detach();

            /*
            if (book && book->has_move()) {
                std::cout << (input == "godeb" ? "A move from the book\n" : "");
                std::cout << "Engine's move: " << book->get_random() << std::endl;
            } else {
                std::thread search([command] { go(command == "godeb"); });
                search.detach();
            }
            */

        } else if (input == "newgame") {
            if (lock) { std::cout << "This command is not available now" << std::endl; continue; }
            board = Board();
            /*
            history.clear();
            if (book)
                book->reset();
            */

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
                if (pos != value.size()) throw std::runtime_error("Incorrect command, try again");

            } catch (const std::exception &e) {
                std::cout << e.what() << std::endl;
                continue;
            }

            if (command == "depth") {
                Search::set_depth(number);
                Search::set_time(INF);
            } else
                Search::set_time(number * 1000);

        } else if (input == "d" || input == "info") {
            if (lock) { std::cout << "This command is not available now" << std::endl; continue; }
            if (input == "d")
                std::cout << board;
            else
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
            std::cout << "newgame - start new game" << std::endl;
            std::cout << "depth n - search for \"n\" nodes in depth" << std::endl;
            std::cout << R"(time n - search for "n" seconds per move or "inf" to disregard time)" << std::endl;
            std::cout << "d - display the current position" << std::endl;
            std::cout << "info - display information about search and more precise about board" << std::endl;
            std::cout << "stop - Instantly stops the search and returns last best move" << std::endl;
            std::cout << "quit - exit the program" << std::endl << std::endl;

            std::cout << "Enter move in coordinate notation, e.g., e4e5, c4e6." << std::endl;
            std::cout << "Or for promotion piece add last symbol q (queen), n (knight), b (bishop) or r (rook)." << std::endl;
            std::cout << "E.g., a7a8q, d2d1r" << std::endl;

        } else if (Move::isMove(input)) {
            if (lock) { std::cout << "This command is not available now" << std::endl; continue; }

            Move move;
            try {
                move = Move(board, command);
            } catch (std::exception &e) {
                std::cout << e.what() << std::endl;
                continue;
            }

            /*
            if (book && book->has_move())
                book->next_move(command);

            if (history.add_pos(board.get_zob_hash()))
                board.make(move);
            else
                std::cout << "Impossible move. Draw" << std::endl;
            if (board.get_ply() == 0)
                history.clear();
            */
        } else
            std::cout << "Incorrect command, try again" << std::endl;
    }
    while (lock); // waiting for finished the thread
}