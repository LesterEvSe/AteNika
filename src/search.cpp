#include "search.hpp"
#include "movegen.hpp"
#include "movepicker.hpp"
#include "eval.hpp"

Move Search::hidden::_best_move;

int16_t Search::hidden::_depth;
int64_t Search::hidden::_nodes;
std::chrono::time_point<std::chrono::steady_clock> Search::hidden::_start;

void Search::init() {
    hidden::_depth = 10;
}

void Search::hidden::_debug(int depth, int best_score, int elapsed) {
    // cp - centi-pawns
    // nps - nodes per second

    std::string score;
    if (best_score == INF)
        score = "mate ";
    else if (best_score == -INF)
        score = "mate -";
    score += std::to_string(best_score);

    // Sometimes I have an error in Linux
    // Process finished with exit code 136 (interrupted by signal 8:SIGFPE)
    // It's divide by zero error, so I increment elapsed ms, to avoid this problem
    std::cout << depth << " nodes: " << (long long)_nodes << "; elapsed: " << elapsed << "ms; ";
    std::cout << "cp: " << score << "; nps: " << (long long)(_nodes*1000 / ++elapsed) << std::endl;
}

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

void Search::iter_deep(Board &board, bool debug) {
    hidden::_best_move = Move();
    hidden::_nodes = 0;
    hidden::_start = std::chrono::steady_clock::now();

    for (int16_t i = 1; i <= hidden::_depth; ++i) {
        int32_t best_score = hidden::_negamax(board, i, -INF, INF);

        int32_t elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - hidden::_start).count();
        if (debug)
            hidden::_debug(i, best_score, elapsed);
    }
}