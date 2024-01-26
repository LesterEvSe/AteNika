#include "pgn_parser.hpp"
#include "movegen.hpp"
#include "move.hpp"

#include <iostream>
#include <fstream>
#include <cerrno>
#include <cstring>
#include <sstream>

std::string PGNParser::hidden::processed_file = "../processed_file.txt";

void PGNParser::hidden::first_processing(const std::string &path) {
    std::ifstream from(path);

    if (!from.is_open())
        throw std::runtime_error(path + " " + strerror(errno));

    std::ofstream to(processed_file, std::ios::out);
    if (!to.is_open())
        throw std::runtime_error(processed_file + " " + strerror(errno));

    std::string prev;
    std::string curr;
    bool add_moves = false;

    auto condition = [](const std::string &str) {
        return str[0] == '3';
    };

    while (std::getline(from, curr)) {
        if (prev.size() > 10 && prev.substr(1, 8) == "WhiteElo" &&
                condition(prev.substr(11, 2)) && condition(curr.substr(11, 2)))
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
}
void PGNParser::hidden::second_processing() {
    std::ifstream from(processed_file);

    if (!from.is_open())
        throw std::runtime_error(processed_file + " " + strerror(errno));

    std::string target = "../book.txt";
    std::ofstream to(target, std::ios::out);
    if (!to.is_open())
        throw std::runtime_error(target + " " + strerror(errno));

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


        int counter = 3;
        while (iss >> command) {
            if (counter >= 300 || command[0] == '{') break;
            if (counter++ % 3 == 0) continue;

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
                std::cerr << "\nTrouble in " + processed_file << std::endl;
                std::cerr << "Line: " << num_line << std::endl;
                std::cerr << "Move: " << counter/3 << ' ' << (counter % 3 == 2 ? "White" : "Black") << std::endl;
                throw std::runtime_error("");
            }
            board.make(move);
        }
        ++num_line;
        to << std::endl;
    }
}

void PGNParser::parse(const std::string &path) {
    try {
        hidden::first_processing(path);
        hidden::second_processing();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}