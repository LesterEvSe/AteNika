#ifndef ATENIKA_BOOK_HPP
#define ATENIKA_BOOK_HPP
#define _CRT_SECURE_NO_WARNINGS // to prevent MSVC compiler error with strerror

#include <string>

#include "book/trie_node.hpp"

class Book {
private:
    std::shared_ptr<TrieNode> head, curr;
    explicit Book(const std::string &path);

public:
    static Book *get_instance(const std::string &path);
    void reset();

    [[nodiscard]] bool has_move() const;
    [[nodiscard]] std::string get_random() const;
    void next_move(const std::string &move);
};

#endif // ATENIKA_BOOK_HPP
