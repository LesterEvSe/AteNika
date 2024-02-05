#include "search.hpp"
#include "movegen.hpp"
#include "movepicker.hpp"
#include "eval.hpp"
#include "ttable.hpp"

Move Search::hidden::_best_move;

int16_t Search::hidden::_depth;
int64_t Search::hidden::_nodes;
std::chrono::time_point<std::chrono::steady_clock> Search::hidden::_start;

void Search::init() {
    hidden::_depth = 4;
}

void Search::hidden::_debug(const Board &board, int depth, int best_score, int elapsed) {
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
    std::cout << depth << " nodes " << (long long)_nodes << " time " << elapsed << "ms ";
    std::cout << "cp " << score << " nps " << (long long)(_nodes*1000 / ++elapsed) << " pv ";

    Board temp = board;
    while (1) {
        Move move = Ttable::get(temp.get_zob_hash());
        if (move.get_flag() == Move::NULL_MOVE)
            break;
        std::cout << (std::string)move << ' ';
        temp.make(move);
    }
    std::cout << std::endl;
}

int32_t Search::hidden::_negamax(Board &board, int16_t ply, int16_t depth, int32_t alpha, int32_t beta) {
    ++_nodes;
    if (depth < 1)
        return Eval::evaluate(board); // quiescence search here

    if (board.get_ply() >= 100)
        return 0;

    MoveList move_list = Movegen(board).get_legal_moves();

    // get size in O(1)
    // checkmate or stalemate
    if (move_list.size() == 0)
        return board.king_in_check(board.get_curr_move()) ? -INF + ply : 0;

//    MovePicker move_picker = MovePicker(&move_list);
    Move curr_best = Move();
    int32_t old_alpha = alpha;

    for (uint8_t i = 0; i < move_list.size(); ++i) {
        Board temp = board;
        temp.make(move_list[i]);

        int32_t score = -_negamax(temp, ply+1, depth-1, -beta, -alpha);

        if (score > alpha) {
            if (score >= beta)
                return beta;
            alpha = score;
            curr_best = move_list[i];
        }
    }

    if (alpha != old_alpha)
        Ttable::add(board.get_zob_hash(), curr_best);

    // here saving move in HashTable
    return alpha;
}

void Search::iter_deep(Board &board, bool debug) {
    hidden::_best_move = Move();
    hidden::_nodes = 0;
    hidden::_start = std::chrono::steady_clock::now();

    for (int16_t i = 1; i <= hidden::_depth; ++i) {
        int32_t best_score = hidden::_negamax(board, 0, i, -INF, INF);

        int32_t elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - hidden::_start).count();
        if (debug)
            hidden::_debug(board, i, best_score, elapsed);
    }
}