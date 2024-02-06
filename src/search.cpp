#include "search.hpp"
#include "movegen.hpp"
#include "movepicker.hpp"
#include "eval.hpp"
#include "ttable.hpp"

int64_t Search::hidden::_nodes;
int32_t Search::hidden::_ms_allocated;
int16_t Search::hidden::_depth;
bool Search::hidden::_stop;

float Search::hidden::_fh; // fail high
float Search::hidden::_fhf; // fail high first

Move Search::hidden::_best_move;
int32_t Search::hidden::_best_score;
std::chrono::time_point<std::chrono::steady_clock> Search::hidden::_start;

std::string Search::hidden::_mate; // for mate check

void Search::init() {
    hidden::_restart();
    hidden::_ms_allocated = 5000;
    hidden::_depth = 7;
}

Move Search::get_best_move() {
    return hidden::_best_move;
}

std::string Search::get_mate() {
    return hidden::_mate;
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
    // moc - move ordering coefficient TODO delete later

    // Sometimes I have an error in Linux
    // Process finished with exit code 136 (interrupted by signal 8:SIGFPE)
    // It's divide by zero error, so I increment elapsed ms, to avoid this problem
    std::cout << depth << " nodes " << (long long)_nodes << " time " << elapsed << "ms ";
    std::cout << score << " nps " << (long long)(_nodes*1000 / ++elapsed) << " moc ";
    std::cout << std::fixed << std::setprecision(2) << _fhf/_fh << " pv ";

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

    _best_move = Move();
}

int32_t Search::hidden::_negamax(Board &board, int16_t ply, int16_t depth, int32_t alpha, int32_t beta)
{
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

    MovePicker move_picker = MovePicker(&move_list);
    Move curr_best = Move();
    int32_t old_alpha = alpha;

    bool first = true;
    ++ply;
    while (move_picker.has_next()) {
        Move move = move_picker.get_next();
        Board temp = board;
        temp.make(move);

        int32_t score = -_negamax(temp, ply, depth-1, -beta, -alpha);

        if (score > alpha) {
            if (score >= beta) {
                if (first) ++_fhf;
                ++_fh;
                return beta;
            }
            alpha = score;
            curr_best = move;
        }
        first = false;
    }

    if (alpha != old_alpha)
        Ttable::add(board.get_zob_hash(), curr_best);

    // here saving move in HashTable
    return alpha;
}

void Search::iter_deep(Board &board, bool debug) {
    hidden::_restart();
    hidden::_start = std::chrono::steady_clock::now();

    for (int16_t i = 1; i <= hidden::_depth; ++i) {
        hidden::_best_score = hidden::_negamax(board, 0, i, -INF, INF);
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