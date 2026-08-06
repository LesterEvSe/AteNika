#include <fstream>
#include <sstream>
#include <cstring> // for strerror

#include "book.hpp"
#include "board.hpp"
#include "move.hpp"

Book::Book(const std::string &path) : head(nullptr) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error(path + " " + strerror(errno));

    head = curr = std::make_shared<TrieNode>();
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string command;

        Board board = Board();
        std::shared_ptr<TrieNode> node = head;

        while (iss >> command) {
            if (!(*node)[command])
                node->add(command);
            node = (*node)[command];

            Move move = Move(board, command);
            board.make(move);
        }
    }
    file.close();
}

Book *Book::get_instance(const std::string &path) {
    static Book instance = Book(path);
    return &instance;
}

void Book::reset() { curr = head; }
bool Book::has_move() const { return curr != nullptr; }
void Book::next_move(const std::string &move) { curr = (*curr)[move]; }
std::string Book::get_random() const { return curr->get_random(); }