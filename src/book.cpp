#include "book.hpp"
#include "board.hpp"
#include "move.hpp"

#include <fstream>
#include <sstream>
#include <cstring> // for strerror

Book::Book(const std::string &path) : head(nullptr) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error(path + " " + strerror(errno));

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string curr;

        Board board = Board();
        while (iss >> curr) {

        }
    }
}

bool Book::has_move() const {
    return head;
}

std::string Book::get_random_next() const {

}