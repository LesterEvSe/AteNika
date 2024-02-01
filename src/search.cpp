#include "search.hpp"
#include "movegen.hpp"
#include "movepicker.hpp"
#include "eval.hpp"

Move Search::hidden::_best_move;
int32_t Search::hidden::_best_score;

int32_t Search::hidden::_nodes;

/*
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
*/

int32_t Search::hidden::_negamax(Board &board, uint16_t depth, int32_t alpha, int32_t beta) {
    ++_nodes;
    if (depth < 1)
        return Eval::evaluate(board);

    MoveList move_list = Movegen(board).get_legal_moves();

    // get size in O(1)
    // checkmate or stalemate
    if (move_list.size() == 0)
        return board.king_in_check(board.get_curr_move()) ? -INF : 0;

    MovePicker move_picker = MovePicker(&move_list);

    Move curr_best = Move();

    while (move_picker.has_next()) {
        Move move = move_picker.get_next();
        Board temp = board;
        temp.make(move);

        int32_t score = -_negamax(temp, depth-1, -beta, -alpha);

        if (score > alpha) {
            if (score >= beta)
                return beta;
            alpha = score;
            curr_best = move;
        }
    }

    _best_move = curr_best;
    // here saving move in HashTable
    return alpha;
}

void Search::iter_deep(Board &board) {
    hidden::_best_move = Move();
    hidden::_nodes = hidden::_best_score = 0;

    hidden::_best_score = hidden::_negamax(board, 7, -INF, INF);
    std::cout << (std::string)hidden::_best_move << ' ' << (int)hidden::_nodes << std::endl;
}