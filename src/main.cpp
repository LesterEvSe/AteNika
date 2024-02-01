#include "bitfunc.hpp"
#include "zobrist_hash.hpp"
#include "rays.hpp"
#include "attacks.hpp"
//#include "mvv_lva.hpp"
//#include "pst.hpp"
//#include "eval.hpp"
//#include "uci.hpp"
//#include "pgn_parser.hpp"

#include <vector>


//std::vector<int> nums {-1, 3, 5, 0, -6, -4, 0, 0};
std::vector<int> nums {8, 5, 6, -4, 3, 8, 4, -6, 1, 0, 5, 2, 0, 0, 0, 0};

// If white, then Max, else Min
int Max(int, int, int, int);
int Min(int, int, int, int);

int NegaMaxNodes = 0;
int CommonNodes = 0;
int NegaMax(int depth, int alpha, int beta, int node, int color) {
    if (depth < 1) {
        ++NegaMaxNodes;
        return color * nums[node];
    }

    for (int i = 0; i < 2; ++i) {
        int score = -NegaMax(depth - 1, -beta, -alpha, (node << 1) | i, -color);
        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;
    }
    return alpha;
}

int Max(int depth, int alpha, int beta, int node) {
    if (depth < 1) {
        ++CommonNodes;
        return nums[node];
    }

    for (int i = 0; i < 2; ++i) {
        int score = Min(depth - 1, alpha, beta, (node << 1) | i);
        if (alpha < score)
            alpha = score;
        if (beta <= alpha)
            return alpha;
    }
    return alpha;
}

int Min(int depth, int alpha, int beta, int node) {
    if (depth < 1) {
        ++CommonNodes;
        return nums[node];
    }

    for (int i = 0; i < 2; ++i) {
        int score = Max(depth - 1, alpha, beta, (node << 1) | i);
        if (score < beta)
            beta = score;
        if (beta <= alpha)
            return beta;
    }
    return beta;
}

int main() {
//    std::cout << NegaMax((int)std::sqrt(nums.size()), -INF, INF, 0, 1) << std::endl;
    std::cout << Max(4, -INF, INF, 0) << ' ';
    std::cout << NegaMax(4, -INF, INF, 0, 1) << std::endl;
    std::cout << CommonNodes << ' ' << NegaMaxNodes;

    return 0;
    init_bits_pre_calculation(); // Must be at the beginning!
    ZobristHash::init();
    Rays::init();
    Attacks::init(); // Must be init after Rays
//    MvvLva::init();
//    PieceTables::init();
//    Eval::init();
//    Search::init();


    // Data take from https://www.ficsgames.org/download.html
    // PGNParser::parse("../ficsgamesdb_2020_CvC_nomovetimes_317502.pgn");
//    Uci::init();
//    Uci::start();
    return 0;
}
