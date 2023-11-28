#include "rays.hpp"
#include "board.hpp"
#include "movepicker.hpp"
#include "pst.hpp"
#include "eval.hpp"
#include "search.hpp"

#include <set>

std::vector<int> scores {
    11, 12, 9, -1, 15, 16, -2, -3, 3, -4, 1, -5, -6, -7, -8, -9
};
std::set<int> visited;

int ab_min(int, int, int, int);

int negamax(int depth, int alpha, int beta, int curr) {
    if (!--depth) {
        visited.emplace(curr);
        return -scores[curr];
    }
    int arr[2] = {(curr << 1), (curr | 1) << 1};

    for (int elem : arr) {
        int score = -negamax(depth, -beta, -alpha, elem);

        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;
    }
    return alpha;
}

int main() {
    Rays::init();
    Attacks::init(); // Must be init after Rays
    MovePicker::init();
    ZobristHash::init();
    PieceTables::init();
    Eval::init();

    int alpha = -100'000;
    int beta  = 100'000;
    std::cout << negamax(4, alpha, beta, 0) << std::endl;
    for (int elem : visited)
        std::cout << elem << ' ';

    return 0;
}
