#include "trie_node.hpp"

std::random_device TrieNode::rd;
std::mt19937 TrieNode::gen(rd());
std::uniform_int_distribution<uint16_t> TrieNode::dist;

std::shared_ptr<TrieNode> TrieNode::operator[](const std::string &move) const {
    int i;
    for (i = 0; i < children.size(); ++i)
        if (children[i].first == move)
            break;
    return i == children.size() ? nullptr : children[i].second;
}

const std::string &TrieNode::get_random() const {
    return children[dist(gen) % children.size()].first;
}

void TrieNode::add(const std::string &move) {
    children.emplace_back(move, std::make_shared<TrieNode>());
}