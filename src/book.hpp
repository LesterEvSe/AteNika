#ifndef ATENIKA_BOOK_HPP
#define ATENIKA_BOOK_HPP

#include <unordered_map>
#include <string>
#include <random>

class Book {
private:
    static constexpr uint64_t PRNG {0xF00DCAFE};
    static std::mt19937 gen;
    static std::uniform_int_distribution<uint16_t> dist;

    struct TrieNode {
        std::unordered_map<std::string, TrieNode*> children;
    };
    TrieNode *head;

public:
    explicit Book(const std::string &path);
    [[nodiscard]] bool has_move() const;
    [[nodiscard]] std::string get_random_next() const;
};

#endif //ATENIKA_BOOK_HPP
