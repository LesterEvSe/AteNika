#include "uci.hpp"
#include "board.hpp"
#include "search.hpp"

#include <sstream>

// Local scope
namespace
{
    Board board;

    void go(std::stringstream &iss) {

    }
}

void Uci::start()
{
    std::cout << "\nAteNika by LesterEvSe\n";
    std::cout << "\"help\" displays all non-uci command" << std::endl << std::endl;

    board = Board();
    bool debug = false;
    std::string input, command;

    while (true) {
        std::getline(std::cin, input);
        std::istringstream iss(input);
        iss >> command;

        if (input == "uci") {
            std::cout << "id name AteNika" << std::endl;
            std::cout << "id author Zhenia Sekhin" << std::endl;
            // here need to output the options that can be changed
            std::cout << "uciok" << std::endl;
        } else if (input == "debug on") {
            debug = true;
        } else if (input == "debug off") {
            debug = false;
        } else if (input == "isready") {
            std::cout << "readyok" << std::endl;
        } /* setoption name [value] here */
        else if (input == "quit") {
            break;
        } else if (input == "help") {
            std::cout << "You can check all uci command on https://www.wbec-ridderkerk.nl/html/UCIProtocol.html" << std::endl;
            std::cout << "Non-uci command" << std::endl;
            std::cout << "d - display the current position" << std::endl;
        } else {
            std::cout << "Unknown command. Type \"help\" for more information." << std::endl;
        }
    }
}