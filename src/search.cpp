#include "search.hpp"
#include "movegen.hpp"
#include "movepicker.hpp"
#include "qmovepicker.hpp"
#include "eval.hpp"
#include "ttable.hpp"

// Limits
int64_t Search::hidden::_nodes;
int32_t Search::hidden::_ms_allocated;
bool Search::hidden::_without_time;
int16_t Search::hidden::_depth;
std::atomic<bool> Search::hidden::_stop;

int64_t Search::hidden::_fh; // fail high
int64_t Search::hidden::_fhf; // fail high first

// Search
OrderInfo Search::hidden::_order_info;
Move Search::hidden::_best_move;
int32_t Search::hidden::_best_score;
std::chrono::time_point<std::chrono::steady_clock> Search::hidden::_start;

std::string Search::hidden::_mate; // for mate check

void Search::hidden::_restart() {
    _nodes = 0;
    _stop = false;

    _fh = 0;
    _fhf = 0;
    _mate = "";

    _order_info = OrderInfo();
    _best_move = Move();
}

void Search::init() {
    hidden::_restart();
    hidden::_ms_allocated = 5000;
    hidden::_without_time = false;
    hidden::_depth = 10;
}

bool Search::hidden::_check_limits() {
    if (_stop) return true;
    // check every 2048 node (if bit 2^11 is set, then check)
    if (_nodes & 2047) return false;

    int32_t elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - hidden::_start).count();

    if (_without_time || elapsed < _ms_allocated) return false;
    return _stop = true;
}
void Search::stop() {
    hidden::_stop = true;
}

std::string Search::get_mate() {
    return hidden::_mate;
}
Move *Search::get_best_move() {
    return hidden::_best_move.get_flag() == Move::NULL_MOVE ? nullptr : &hidden::_best_move;
}
std::string Search::get_allocated_sec() {
    return hidden::_without_time ? "infinity" : std::to_string(hidden::_ms_allocated/1000) + " sec";
}
int32_t Search::get_search_depth() {
    return hidden::_depth;
}

void Search::set_time(int32_t ms_allocated) {
    if (ms_allocated == INF)
        hidden::_without_time = true;
    else if (ms_allocated > 0) {
        hidden::_without_time = false;
        hidden::_ms_allocated = ms_allocated;
    }
}

void Search::set_depth(int16_t depth) {
    if (depth > 0)
        hidden::_depth = depth;
}


void Search::hidden::_debug(const Board &board, int depth, int elapsed)
{
    std::string score;
    if (hidden::_best_score > 2'000'000'000)
        score = (board.get_curr_move() == WHITE ? "WM" : "BM") + std::to_string(INF - hidden::_best_score);
    else
        score = "cp " + std::to_string(hidden::_best_score);

    // cp  - centi-pawns
    // nps - nodes per second
    // moq - move ordering quality

    // Sometimes I have an error in Linux
    // Process finished with exit code 136 (interrupted by signal 8:SIGFPE)
    // It's divide by zero error, so I increment elapsed ms, to avoid this problem
    std::cout << depth << " nodes " << (long long)_nodes;
    std::cout << " time " << elapsed << "ms ";
    std::cout << score << " nps " << (long long)(_nodes*1000 / ++elapsed);

    std::cout << " moq " << (int)(100.0 * _fhf/ (_fh == 0 ? 1 : _fh)) << '%';
    std::cout << " pv ";

    Board temp = board;
    // Set a counter, so we don't go over the limit
    for (int16_t i = 0; i < depth && Ttable::in_table(temp.get_zob_hash()); ++i) {
        Move move = Ttable::get(temp.get_zob_hash());
        std::cout << (std::string)move << ' ';
        temp.make(move);
    }
    std::cout << std::endl;
}

void Search::iter_deep(Board &board, bool debug) {
    hidden::_restart();
    hidden::_start = std::chrono::steady_clock::now();

    for (int16_t i = 1; i <= hidden::_depth; ++i) {
        hidden::_best_score = hidden::_negamax(board, i, -INF, INF);

        int32_t elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - hidden::_start).count();
        if (hidden::_stop)
            break;
        if (debug)
            hidden::_debug(board, i, elapsed);

        hidden::_best_move = Ttable::get(board.get_zob_hash());
        if (hidden::_best_score > 2'000'000'000) {
            // for testing
            hidden::_mate = (board.get_curr_move() == WHITE ? "WM" : "BM") + std::to_string(INF - hidden::_best_score);
            break;
        }

        if (!hidden::_without_time && elapsed >= (hidden::_ms_allocated / 2))
            break;
    }
}

int32_t Search::hidden::_negamax(Board &board, int16_t depth, int32_t alpha, int32_t beta)
{
    if (_check_limits())
        return 0;
    if (depth < 1)
        return _quiescence(board, alpha, beta);
//        return Eval::evaluate(board); // quiescence search here

    ++_nodes;
    if (board.get_ply() >= MAX_PLY || History::threefold_rule(board))
        return 0;

    MoveList move_list = Movegen(board).get_legal_moves();

    // get size in O(1)
    // checkmate or stalemate
    if (move_list.size() == 0)
        return board.king_in_check(board.get_curr_move()) ? -INF + _order_info.get_ply() : 0;

    ZobristHash zob_hash = board.get_zob_hash();
    MovePicker move_picker = MovePicker(&move_list, board.get_zob_hash(), _order_info);

    Move curr_best = Move();
    int32_t old_alpha = alpha;

    // PVS - Principal Variation Search
    // https://www.chessprogramming.org/Principal_Variation_Search
    bool full_window = true;
    bool first_move = true;
    ++_order_info;

    while (move_picker.has_next()) {
        Move move = move_picker.get_next();
        board.make(move);

        int32_t score;
        if (full_window)
            score = -_negamax(board, depth-1, -beta, -alpha);
        else {
            score = -_negamax(board, depth-1, -alpha-1, -alpha);
            if (score > alpha)
                score = -_negamax(board, depth-1, -beta, -alpha);
        }
        board.unmake(move);

        if (score > alpha) {
            if (score >= beta) {
                --_order_info;
                if (first_move) ++_fhf;
                ++_fh;

                if (!(move.get_flag() & Move::CAPTURE))
                    _order_info.add_killer(move);
                return beta;
            }
            alpha = score;
            curr_best = move;
            full_window = false;

            if (!(move.get_flag() & Move::CAPTURE))
                _order_info.add_history(curr_best.get_from_cell(), curr_best.get_to_cell(), depth);
        }
        first_move = false;
    }
    --_order_info;

    if (alpha != old_alpha)
        Ttable::add(zob_hash, curr_best);
    return alpha;
}

int32_t Search::hidden::_quiescence(Board &board, int32_t alpha, int32_t beta)
{
    // check limits here
    if (_check_limits())
        return 0;

    ++_nodes;
    if (board.get_ply() >= MAX_PLY || History::threefold_rule(board))
        return 0;

    // https://www.chessprogramming.org/Quiescence_Search#Standing_Pat
    int32_t stand_pat = Eval::evaluate(board);
    if (stand_pat >= beta)
        return beta;
    if (stand_pat > alpha)
        alpha = stand_pat;

    MoveList move_list = Movegen(board).get_legal_moves();
    if (move_list.size() == 0)
        return board.king_in_check(board.get_curr_move()) ? -INF + _order_info.get_ply() : 0;

    ZobristHash zob_hash = board.get_zob_hash();
    QMovePicker q_move_picker = QMovePicker(&move_list, zob_hash);

    Move curr_best = Move();
    int32_t old_alpha = alpha;
    bool first_move = true;
    ++_order_info;

    while (q_move_picker.has_next())
    {
        Move move = q_move_picker.get_next();
        board.make(move);
        stand_pat = -_quiescence(board, -beta, -alpha);
        board.unmake(move);

        if (stand_pat > alpha) {
            if (stand_pat >= beta) {
                --_order_info;
                if (first_move) ++_fhf;
                ++_fh;
                return beta;
            }
            alpha = stand_pat;
            curr_best = move;
        }
        first_move = false;
    }
    --_order_info;

    if (alpha != old_alpha)
        Ttable::add(zob_hash, curr_best);
    return alpha;
}