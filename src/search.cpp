#include "search.hpp"
#include "movegen.hpp"
#include "movepicker.hpp"
#include "Ttable.hpp"

#include <chrono>

Move Search::hidden::_best_move;
int32_t Search::hidden::_best_score;
History Search::hidden::_history;
bool Search::hidden::_stop;

void Search::restart() {
    hidden::_history = History();
    hidden::_stop = false;
}

void Search::stop() {
    hidden::_stop = true;
}

void Search::iter_deep(const Board &board) {
    for (int16_t i = 0; i < hidden::MAX_DEPTH; ++i) {
        if (hidden::_stop)
            break;

        MoveList moves = Movegen(board).get_legal_moves();
        MovePicker move_picker = MovePicker(&moves);

        int32_t alpha = -hidden::INF;
        int32_t beta = hidden::INF;

        while (move_picker.has_next()) {
            Move move = move_picker.get_next();
            Board temp = board;
            temp.make(move);
            int32_t score = -hidden::_negamax(temp, hidden::MAX_DEPTH, alpha, beta);

            if (score > alpha) {
                alpha = score;
                hidden::_best_move = move;

                // if checkmate
                if (score == hidden::INF)
                    break;
            }
        }
    }
}

//int negamax(int depth, int alpha, int beta, int curr) {
//    if (!--depth) {
//        visited.emplace(curr);
//        return -scores[curr];
//    }
//    int arr[2] = {(curr << 1), (curr | 1) << 1};
//
//    for (int elem : arr) {
//        int score = -negamax(depth, -beta, -alpha, elem);
//
//        if (score >= beta)
//            return beta;
//        if (score > alpha)
//            alpha = score;
//    }
//    return alpha;
//}

int32_t Search::hidden::_negamax(const Board &board, int16_t depth, int32_t alpha, int32_t beta) {
    if (_stop)
        return 0;

    ZobristHash zob_hash = board.get_zob_hash();
    _history.add_pos(zob_hash);

    if (board.get_ply() >= DRAW_RULE_50)
        return 0;
    if (board.get_ply() == 0)
        _history.clear();
    else if (_history.rule_of_threes(zob_hash))
        return 0;

    // MAX_DEPTH - depth (curr depth)
    // if curr_depth + TT depth >= MAX_DEPTH, then use, otherwise recalculate
    // MD - depth + TT_depth >= MD simplified to TT_depth >= d
    // TODO add -> && Transposition::get(zob_hash).get_depth() >= depth
    if (Transposition::in_table(zob_hash)) {
        TTEntry entry = Transposition::get(zob_hash);
        int32_t score = entry.get_score();

        switch (entry.get_flag()) {
            case LOWER : alpha = score < alpha ? alpha : score; // maximum
            case EXACT : return entry.get_score();
            case UPPER : beta = score < beta ? score : beta; // minimum
        }

        // Beta-cutoff condition
        if (alpha >= beta)
            return score;
    }

    MoveList move_list = Movegen(board).get_legal_moves();

    // The complexity of this method is O(1)
    // In this case, checkmate or stalemate
    if (move_list.size() == 0)
        return board.king_in_check(board.get_curr_move()) ? -INF : 0;

    // Adding depth if it's now king in check.
    // We can do this because it won't increase the search tree much


    // if depth == 0, the quiescence search

    MovePicker move_picker = MovePicker(&move_list);
    while (move_picker.has_next()) {
        Move move = move_picker.get_next();
        Board temp = board;
        temp.make(move);

//        int32_t score = -_negamax()
    }

    // set new TT
    return alpha;
}