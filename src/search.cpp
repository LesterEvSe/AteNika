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

std::atomic<bool> Search::hidden::_stop;
int16_t Search::hidden::_check_gap = GAP;

std::chrono::time_point<std::chrono::steady_clock> Search::hidden::_start;
int16_t Search::hidden::_seeking_depth;

void Search::init() {
    hidden::_nodes = 0;
    hidden::_best_score = 0;
    hidden::_best_move = Move();
    hidden::_history = History();

    hidden::_time_allocated_ms = 3 * 1000;
    hidden::_without_time = false;
    hidden::_stop = false;
    hidden::_seeking_depth = 20;
}

void Search::stop() {
    hidden::_stop = true;
}

void Search::hidden::new_search() {
    _nodes = 0;
    _best_score = 0;
    _best_move = Move();
    _stop = false;
}

bool Search::hidden::check_limits() {
    if (_stop) return true;
    if (--_check_gap > 0) return false;

    _check_gap = GAP;
    int32_t elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - hidden::_start).count();

    if (_without_time || elapsed < _time_allocated_ms) return false;
    return _stop = true;
}

Move *Search::get_best_move() {
    return hidden::_best_move.get_flag() == Move::NULL_MOVE ? nullptr : &hidden::_best_move;
}

int32_t Search::get_score() {
    return hidden::_best_score;
}

bool Search::set_time(int32_t time_allocated_ms) {
    if (time_allocated_ms == INF) {
        hidden::_without_time = true;
        return true;
    } else if (time_allocated_ms < 1 || time_allocated_ms > 2'000'000)
        return false;
    else {
        hidden::_without_time = false;
        hidden::_time_allocated_ms = time_allocated_ms;
        return true;
    }
}

bool Search::set_depth(int16_t n) {
    if (n < 1 || n > 50) return false;
    hidden::_seeking_depth = n;
    return true;
}

void Search::iter_deep(const History &history, const Board &board, bool debug) {
    hidden::new_search();
    hidden::_history = history;
    hidden::_start = std::chrono::steady_clock::now();

    for (int16_t i = 1; i <= hidden::_seeking_depth; ++i) {

        MoveList legal_moves = Movegen(board).get_legal_moves();
        if (legal_moves.size() == 0) {
            hidden::_best_move = Move();
            hidden::_best_score = board.king_in_check(board.get_curr_move()) ? -INF : 0;
            return;
        }
        MovePicker move_picker = MovePicker(&legal_moves, board);

        int32_t alpha = -INF;
        int32_t beta = INF;

        Move curr_best_move = legal_moves[0];
        while (move_picker.has_next()) {
            Move move = move_picker.get_next();
            Board temp = board;
            temp.make(move);
            int32_t score = -hidden::_negamax(temp, i, -beta, -alpha);

            if (hidden::check_limits())
                break;

            if (score > alpha) {
                alpha = score;
                curr_best_move = move;

                // if checkmate
                if (score == INF)
                    break;
            }
        }

        int32_t elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - hidden::_start).count();
        if (debug)
            std::cout << (int)i << " nodes: " << (long long)hidden::_nodes << "; elapsed: " << (int)elapsed << "ms" << std::endl;

        if (hidden::_stop)
            break;

        TTEntry entry = TTEntry(curr_best_move, alpha, i, EXACT);
        Transposition::set(board.get_zob_hash(), entry);

        hidden::_best_move = curr_best_move;
        hidden::_best_score = alpha;

        if (!hidden::_without_time && elapsed >= (hidden::_time_allocated_ms / 2))
            break;
    }
}

int32_t Search::hidden::_negamax(const Board &board, int16_t depth, int32_t alpha, int32_t beta) {
    ++_nodes;
    if (check_limits())
        return 0;

    ZobristHash zob_hash = board.get_zob_hash();
    if (board.get_ply() >= DRAW_RULE_50 || _history.rule_of_threes(zob_hash))
        return 0;

    // _seeking_depth - depth (curr depth)
    // if curr_depth + TT depth >= _seeking_depth, then use, otherwise recalculate
    // MD - depth + TT_depth >= MD simplified to TT_depth >= d
    if (Transposition::in_table(zob_hash) && Transposition::get(zob_hash).get_depth() >= depth) {
        TTEntry entry = Transposition::get(zob_hash);
        int32_t score = entry.get_score();

        switch (entry.get_flag()) {
            case LOWER : alpha = score < alpha ? alpha : score; break; // max
            case EXACT : return score;
            case UPPER : beta = score < beta ? score : beta; break; // min
        }

        // Beta-cutoff condition
        if (alpha >= beta)
            return score;
    }

    MoveList legal_moves = Movegen(board).get_legal_moves();

    // We can increase depth, when king in check,
    // because it won't increase the search tree much
    int32_t king_threat = board.king_in_check(board.get_curr_move());

    // The complexity of this method is O(1)
    // In this case, checkmate or stalemate
    if (legal_moves.size() == 0)
        return king_threat ? -INF : 0;

    if ((depth + king_threat) <= 0)
        return _quiescence_search(board, alpha, beta);

    MovePicker move_picker = MovePicker(&legal_moves, board);
    Move curr_best_move = legal_moves[0];
    int32_t start_alpha = alpha;
    bool full_window = true;

    while (move_picker.has_next()) {
        Move move = move_picker.get_next();
        Board temp = board;
        temp.make(move);

        int32_t score;
        if (full_window)
            score = -_negamax(temp, depth - 1 + king_threat, -beta, -alpha);
        else {
            score = -_negamax(temp, depth - 1 + king_threat, -alpha - 1, -alpha);
            if (score > alpha)
                score = -_negamax(temp, depth - 1 + king_threat, -beta, -alpha);
        }
        if (score >= beta) {
            TTEntry entry = TTEntry(move, score, depth, LOWER);
            Transposition::set(temp.get_zob_hash(), entry);
            return beta;
        }

        // new best move
        if (score > alpha) {
            full_window = false;
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
    if (check_limits())
        return 0;

    MoveList legal_moves = Movegen(board).get_legal_moves();

    // Stalemate if not in check
    if (legal_moves.size() == 0)
        return board.king_in_check(board.get_curr_move()) ? -INF : 0;

    int32_t static_eval = Eval::evaluate(board, board.get_curr_move());
    ++_nodes;

    QMovePicker q_move_picker = QMovePicker(&legal_moves);

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