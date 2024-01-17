#include "search.hpp"
#include "movegen.hpp"
#include "movepicker.hpp"
#include "quiescence_movepick.hpp"
#include "Ttable.hpp"
#include "eval.hpp"

#include <chrono>

Move Search::hidden::_best_move;
int32_t Search::hidden::_best_score;
int64_t Search::hidden::_nodes;
History Search::hidden::_history;

int32_t Search::hidden::_time_allocated_ms;
bool Search::hidden::_without_time;
bool Search::hidden::_stop;

void Search::restart() {
    hidden::_nodes = 0;
    hidden::_best_move = Move();
    hidden::_best_score = 0;
    hidden::_history = History();

    hidden::_time_allocated_ms = 0;
    hidden::_without_time = true;
    hidden::_stop = false;
}

void Search::stop() {
    hidden::_stop = true;
}

Move Search::get_best_move() {
    return hidden::_best_move;
}

void Search::set_time(int32_t time_allocated_ms) {
    if (time_allocated_ms == INF)
        Search::hidden::_without_time = true;
    else
        Search::hidden::_time_allocated_ms = time_allocated_ms;
}

void Search::iter_deep(const Board &board, bool debug) {
    auto start = std::chrono::steady_clock::now();

    for (int16_t i = 1; i <= hidden::MAX_DEPTH; ++i) {
        if (hidden::_stop)
            break;

        MoveList moves = Movegen(board).get_legal_moves();
        MovePicker move_picker = MovePicker(&moves, board);

        int32_t alpha = -INF;
        int32_t beta = INF;

        Move curr_best_move = moves[0];
        while (move_picker.has_next()) {
            Move move = move_picker.get_next();
            Board temp = board;
            temp.make(move);
            int32_t score = -hidden::_negamax(temp, i, alpha, beta);

            if (score > alpha) {
                alpha = score;
                curr_best_move = move;

                // if checkmate
                if (score == INF)
                    break;
            }
        }

        int32_t elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
        if (debug)
            std::cout << (int)i << " nodes: " << (long long)hidden::_nodes << "; elapsed: " << (int)elapsed << "ms" << std::endl;

        if (!hidden::_stop) {
            TTEntry entry = TTEntry(curr_best_move, alpha, i, EXACT);
            Transposition::set(board.get_zob_hash(), entry);

            hidden::_best_move = curr_best_move;
            hidden::_best_score = alpha;
        }
        if (!hidden::_without_time && elapsed >= (hidden::_time_allocated_ms / 2))
            break;
    }
    hidden::_stop = false; // clear stop flag
}

int32_t Search::hidden::_negamax(const Board &board, int16_t depth, int32_t alpha, int32_t beta) {
    ++_nodes;
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
    if (Transposition::in_table(zob_hash) && Transposition::get(zob_hash).get_depth() >= depth) {
        TTEntry entry = Transposition::get(zob_hash);
        int32_t score = entry.get_score();

        switch (entry.get_flag()) {
            case LOWER : alpha = std::max(score, alpha); break; // score < alpha ? alpha : score; break; // maximum
            case EXACT : return entry.get_score();
            case UPPER : beta = std::min(score, beta); break; // score < beta ? score : beta; break; // minimum
        }

        // Beta-cutoff condition
        if (alpha >= beta)
            return score;
    }

    MoveList legal_moves = Movegen(board).get_legal_moves();

    // The complexity of this method is O(1)
    // In this case, checkmate or stalemate
    if (legal_moves.size() == 0)
        return board.king_in_check(board.get_curr_move()) ? -INF : 0;

    // We can increase depth, when king in check,
    // because it won't increase the search tree much
    if (board.king_in_check(board.get_curr_move()))
        ++depth;

    if (--depth == 0) {
        return Eval::evaluate(board, board.get_curr_move());
        return _quiescence_search(board, -beta, -alpha);
    }

    MovePicker move_picker = MovePicker(&legal_moves, board);
    Move curr_best_move = legal_moves[0];
    int32_t start_alpha = alpha;

    while (move_picker.has_next()) {
        Move move = move_picker.get_next();
        Board temp = board;
        temp.make(move);

        int32_t score = -_negamax(temp, depth, -beta, -alpha);
        if (score >= beta) {
            TTEntry entry = TTEntry(move, score, depth, LOWER);
            Transposition::set(temp.get_zob_hash(), entry);
            return beta;
        }

        // new best move
        if (score > alpha) {
            alpha = score;
            curr_best_move = move;
        }
    }

    TTFlag flag = (alpha > start_alpha) ? EXACT : UPPER;
    TTEntry entry = TTEntry(curr_best_move, alpha, depth, flag);
    Transposition::set(zob_hash, entry);
    return alpha;
}

int32_t Search::hidden::_quiescence_search(const Board &board, int32_t alpha, int32_t beta) {
    ++_nodes;
    if (_stop)
        return 0;

    MoveList legal_moves = Movegen(board).get_legal_moves();

    // Stalemate if not in check
    if (legal_moves.size() == 0)
        return board.king_in_check(board.get_curr_move()) ? -INF : 0;

    QMovePicker q_move_picker = QMovePicker(&legal_moves);

    int32_t static_eval = Eval::evaluate(board, board.get_curr_move());

    // if node already quite
    if (!q_move_picker.has_next())
        return static_eval;
    if (static_eval >= beta)
        return beta;
    if (static_eval > alpha)
        alpha = static_eval;

    while (q_move_picker.has_next()) {
        Board temp = board;
        temp.make(q_move_picker.get_next());

        int32_t score = -_quiescence_search(temp, -beta, -alpha);
        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;
    }
    return alpha;
}