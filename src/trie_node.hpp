#ifndef ATENIKA_TRIE_NODE_HPP
#define ATENIKA_TRIE_NODE_HPP

#include <vector>
#include <string>
#include <random>
#include <memory>

class TrieNode {
private:
    static std::random_device rd; // random device
    static std::mt19937 gen;
    static std::uniform_int_distribution<uint16_t> dist;
    std::vector<std::pair<std::string, std::shared_ptr<TrieNode>>> children;

public:
    TrieNode() = default;
    std::shared_ptr<TrieNode> operator[](const std::string &move) const;
    [[nodiscard]] const std::string &get_random() const;
    void add(const std::string &move);
};

#endif //ATENIKA_TRIE_NODE_HPP
