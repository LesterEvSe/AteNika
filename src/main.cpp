#include "rays.hpp"
#include "mvv_lva.hpp"
#include "pst.hpp"
#include "eval.hpp"
#include "board.hpp"
#include "uci.hpp"

#include <fstream>
#include "movegen.hpp"
#include "move.hpp"

int convert_first() {
    std::ifstream from("../all.txt");

    if (!from.is_open()) {
        std::cout << "Error open 'from' file" << std::endl;
        return 1;
    }

    std::ofstream to("../all_new.txt", std::ios::out);
    if (!to.is_open()) {
        std::cout << "Error open 'to' file" << std::endl;
        return 1;
    }

    std::string prev;
    std::string curr;
    bool add_moves = false;

    auto pattern = [](const std::string &str) {
        return str[0] == '3';// || str[0] == '2' && str[1] > '8';
    };
    while (std::getline(from, curr)) {
        if (prev.size() > 10 && prev.substr(1, 8) == "WhiteElo" &&
            pattern(prev.substr(11, 2)) && pattern(curr.substr(11, 2)))
        {
            to << prev << std::endl;
            to << curr << std::endl;
            add_moves = true;
        }
        if (!curr.empty() && curr[0] == '1' && add_moves) {
            to << curr << std::endl;
            add_moves = false;
        }
        prev = curr;
    }

    from.close();
    to.close();
    return 0;
}

int convert_second() {
    std::ifstream from("../all_new.txt");

    if (!from.is_open()) {
        std::cout << "Error open 'from' file" << std::endl;
        return 1;
    }

    std::ofstream to("../res.txt", std::ios::out);
    if (!to.is_open()) {
        std::cout << "Error open 'to' file" << std::endl;
        return 1;
    }

    std::string line;
    int num_line = 1;
    while (std::getline(from, line)) {
        std::istringstream iss(line);

        Board board = Board();
        std::string command;
        if (line[0] == '[') {
            ++num_line;
            continue;
        }


        int coun = 3;
        while (iss >> command) {
            std::cout << command << std::endl;
            if (coun >= 300 || command[0] == '{') break;
            if (coun++ % 3 == 0) continue;

            if (command.back() == '+' || command.back() == '#')
                command.pop_back();
            if (command[1] == 'x')
                command.erase(1, 1);
            if (command.size() == 4 && command[2] == '=')
                command[3] = tolower(command[3]);
            if (command.size() == 5 && command[3] == '=')
                command[4] = tolower(command[4]);
            if (command.size() == 5 && command[2] == 'x')
                command.erase(2, 1);


            MoveList moves = Movegen(board).get_legal_moves();
            Move move;

            int i;
            for (i = 0; i < moves.size(); ++i) {
                move = moves[i];
                std::string temp = (std::string)moves[i];

                if (command.size() == 2) {
                    if (temp.substr(2, 2) == command) {
                        to << temp << ' ';
                        break;
                    }
                    continue;
                }

                if (command == "O-O" && moves[i].get_flag() == Move::Flag::KSIDE_CASTLING) {
                    to << temp << ' ';
                    break;
                }
                if (command == "O-O-O" && moves[i].get_flag() == Move::Flag::QSIDE_CASTLING) {
                    to << temp << ' ';
                    break;
                }

                if (command[2] == '=') {
                    if (command.substr(0, 2) == temp.substr(2, 2) && command[3] == temp[4]) {
                        to << temp << ' ';
                        break;
                    }
                    continue;
                }
                if (command[3] == '=') {
//                    std::cout << command << ' ' << temp << std::endl;
                    if (command.substr(1, 2) == temp.substr(2, 2) && command[4] == temp[4] && command[0] == temp[0]) {
                        to << temp << ' ';
                        break;
                    }
                    continue;
                }

                if (command.size() == 3) {
                    PieceType type;
                    switch (command[0]) {
                        case 'Q': type = QUEEN; break;
                        case 'N': type = KNIGHT; break;
                        case 'B': type = BISHOP; break;
                        case 'R': type = ROOK; break;
                        case 'K': type = KING; break;
                        default: type = PAWN; break;
                    }

                    if (type == PAWN) {
                        if (command[0] == temp[0] && command.substr(1, 2) == temp.substr(2, 2)) {
                            to << temp << ' ';
                            break;
                        }
                        continue;
                    }

                    if (moves[i].get_move_piece() == type && command.substr(1, 2) == temp.substr(2, 2)) {
                        to << temp << ' ';
                        break;
                    }
                }
                if (command.size() == 4) {
                    PieceType type;
                    switch (command[0]) {
                        case 'Q': type = QUEEN; break;
                        case 'N': type = KNIGHT; break;
                        case 'B': type = BISHOP; break;
                        case 'R': type = ROOK; break;
                        case 'K': type = KING; break;
                        default: type = PAWN; break;
                    }

                    int ind = isdigit(command[1]);
                    if (command[1] != temp[ind] || moves[i].get_move_piece() != type) continue;
                    if (command.substr(2, 2) == temp.substr(2, 2)) {
                        to << temp << ' ';
                        break;
                    }
                }
            }

            if (i == moves.size()) {
                std::cout << "Trouble in" << std::endl;
                std::cout << "Line: " << num_line << std::endl;
                std::cout << "Move: " << coun/3 << ' ' << (coun % 3 == 2 ? "White" : "Black") << std::endl;
                return 1;
            }
            board.make(move);
        }
        ++num_line;
        to << std::endl;
    }

    return 0;
}

#include "pgn_parser.hpp"

int main() {
    init_bits_pre_calculation(); // Must be at the beginning!
    ZobristHash::init();
    Rays::init();
    Attacks::init(); // Must be init after Rays
    MvvLva::init();
    PieceTables::init();
    Eval::init();
    Search::init();

    // Data take from https://www.ficsgames.org/download.html
    PGNParser::parse("../all.txt");
    // PGNParser::parse("../ficsgamesdb_2020_CvC_nomovetimes_317393.pgn");
//    Uci::init();
//    Uci::start();
    return 0;
}
