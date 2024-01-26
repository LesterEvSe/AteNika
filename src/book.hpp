#ifndef ATENIKA_BOOK_HPP
#define ATENIKA_BOOK_HPP

#include "trie_node.hpp"
#include <string>
#include <random>

class Book {
private:
    std::shared_ptr<TrieNode> head;

public:
    explicit Book(const std::string &path);
    [[nodiscard]] bool has_move() const;

    [[nodiscard]] std::string get_random() const;
    void next_move(const std::string &move);
};

#endif //ATENIKA_BOOK_HPP
