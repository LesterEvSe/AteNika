#include "book.hpp"
#include "board.hpp"
#include "move.hpp"
#include "Ttable.hpp"
#include "eval.hpp"

#include <fstream>
#include <sstream>
#include <cstring> // for strerror

Book::Book(const std::string &path) : head(nullptr) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error(path + " " + strerror(errno));

    head = std::make_shared<TrieNode>();
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string command;

        Board board = Board();
        std::shared_ptr<TrieNode> curr = head;

        while (iss >> command) {
            Move move = Move(board, command);
            ZobristHash temp = board.get_zob_hash();
            board.make(move);

            if (!(*curr)[command])
                curr->add(command);
            curr = (*curr)[command];

            // The depth is great enough that it could be considered the best move ever
            if (Transposition::in_table(temp)) continue;
            TTEntry entry = TTEntry(move, Eval::evaluate<Eval::STATIC>(board, board.get_curr_move()), 300, EXACT);
            Transposition::set(board.get_zob_hash(), entry);
        }
    }
    file.close();
}

bool Book::has_move() const                     { return head != nullptr;    }
void Book::next_move(const std::string &move)   { head = (*head)[move];      }
std::string Book::get_random() const            { return head->get_random(); }