#include "search.hpp"
#include "movegen.hpp"
#include "movepicker.hpp"
#include "eval.hpp"
#include "ttable.hpp"

int64_t Search::hidden::_nodes;
int32_t Search::hidden::_ms_allocated;
int16_t Search::hidden::_depth;
bool Search::hidden::_stop;

int64_t Search::hidden::_fh; // fail high
int64_t Search::hidden::_fhf; // fail high first

OrderInfo Search::hidden::_order_info;
Move Search::hidden::_best_move;
int32_t Search::hidden::_best_score;
std::chrono::time_point<std::chrono::steady_clock> Search::hidden::_start;

std::string Search::hidden::_mate; // for mate check

void Search::init() {
    hidden::_restart();
    hidden::_ms_allocated = 5000;
    hidden::_depth = 5;
}

Move Search::get_best_move() {
    return hidden::_best_move;
}

std::string Search::get_mate() {
    return hidden::_mate;
}

bool Search::set_depth(int16_t depth) {
    if (depth < 0 || depth > 100) return false;
    hidden::_depth = depth;
    return true;
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
    // moq - move ordering quality TODO delete later

    // Sometimes I have an error in Linux
    // Process finished with exit code 136 (interrupted by signal 8:SIGFPE)
    // It's divide by zero error, so I increment elapsed ms, to avoid this problem
    std::cout << depth << " nodes " << (long long)_nodes << " time " << elapsed << "ms ";
    std::cout << score << " nps " << (long long)(_nodes*1000 / ++elapsed);

    std::cout << " moq " << (int)(100.0 * _fhf/_fh) << '%';
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

void Search::hidden::_restart() {
    _nodes = 0;
    _stop = false;

    // 1, to exclude divide by zero
    _fh = 1;
    _fhf = 0;
    _mate = "";

    _order_info = OrderInfo();
    _best_move = Move();
}

int32_t Search::hidden::_negamax(Board &board, int16_t depth, int32_t alpha, int32_t beta)
{
    ++_nodes;
    if (depth < 1)
        return Eval::evaluate(board); // quiescence search here

    if (board.get_ply() >= MAX_PLY)
        return 0;

    MoveList move_list = Movegen(board).get_legal_moves();

    // get size in O(1)
    // checkmate or stalemate
    if (move_list.size() == 0)
        return board.king_in_check(board.get_curr_move()) ? -INF + _order_info.get_ply() : 0;

    // TODO Test Killers heuristic
    MovePicker move_picker = MovePicker(&move_list, _order_info);
    Move curr_best = Move();
    int32_t old_alpha = alpha;

    // PVS - Principal Variation Search
    // https://www.chessprogramming.org/Principal_Variation_Search
    bool full_window = true;
    bool first_move = true;
    ++_order_info;

    while (move_picker.has_next()) {
        Move move = move_picker.get_next();
        Board temp = board;
        temp.make(move);

        int32_t score;
        if (full_window)
            score = -_negamax(temp, depth-1, -beta, -alpha);
        else {
            score = -_negamax(temp, depth-1, -alpha-1, -alpha);
            if (score > alpha)
                score = -_negamax(temp, depth-1, -beta, -alpha);
        }

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
        Ttable::add(board.get_zob_hash(), curr_best);
    return alpha;
}

void Search::iter_deep(Board &board, bool debug) {
    hidden::_restart();
    hidden::_start = std::chrono::steady_clock::now();

    for (int16_t i = 1; i <= hidden::_depth; ++i) {
        hidden::_best_score = hidden::_negamax(board, i, -INF, INF);
        hidden::_best_move = Ttable::get(board.get_zob_hash());

        int32_t elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - hidden::_start).count();
        if (debug)
            hidden::_debug(board, i, elapsed);

        if (hidden::_best_score > 2'000'000'000) {
            hidden::_mate = (board.get_curr_move() == WHITE ? "WM" : "BM") + std::to_string(INF - hidden::_best_score);
            break;
        }
    }
}