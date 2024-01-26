#include "pgn_parser.hpp"
#include "movegen.hpp"
#include "move.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring> // for strerror
#include <functional>

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
        std::swap(prev, curr);
    }
    from.close();
    to.close();
}

namespace {
    std::string processing(const std::string &input) {
        std::string command = input;
        int j = 0;
        for (int i = 0; i < command.size(); ++i) {
            if (command[i] == 'x' || command[i] == '+' || command[i] == '#') continue;
            command[j++] = command[i];
        }
        command.resize(j--);

        if (command[j-1] == '=')
            command[j] = tolower(command[j]);
        return command;
    }


    bool size2_check(const std::string &command, const Move &move) {
        return command == static_cast<std::string>(move).substr(2, 2);
    }
    bool kside_castling_check(const std::string &command, const Move &move) {
        return move.get_flag() == Move::KSIDE_CASTLING;
    }
    bool qside_castling_check(const std::string &command, const Move &move) {
        return move.get_flag() == Move::QSIDE_CASTLING;
    }
    bool promotion_check(const std::string &com, const Move &move) {
        bool ind3 = com[2] != '=';
        std::string temp = static_cast<std::string>(move);
        return com.substr(ind3, 2) == temp.substr(2, 2) && com[3+ind3] == temp[4] && (!ind3 || com[0] == temp[0]);
    }
    bool other_check(const std::string &command, const Move &move) {
        std::string temp = static_cast<std::string>(move);
        PieceType type;
        switch (command[0]) {
            case 'Q': type = QUEEN; break;
            case 'N': type = KNIGHT; break;
            case 'B': type = BISHOP; break;
            case 'R': type = ROOK; break;
            case 'K': type = KING; break;
            default: type = PAWN; break;
        }
        std::string last = temp.substr(2, 2);

        if (bool digit = isdigit(command[1]); command.size() == 4)
            return command[1] == temp[digit] && move.get_move_piece() == type && command.substr(2, 2) == last;

        // here size 3
        if (type == PAWN)
            return command[0] == temp[0] && command.substr(1, 2) == last;
        return move.get_move_piece() == type && command.substr(1, 2) == last;
    }
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

        // We have pattern: number of move, white move, black move. So first move is 3/3 = 1
        int counter = 3;
        while (iss >> command) {
            if (counter >= 300 || command[0] == '{') break;
            if (counter++ % 3 == 0) continue;

            command = processing(command);
            std::function<bool(const std::string &, const Move &)> check;

            if (command.size() == 2)
                check = size2_check;
            else if (command == "O-O")
                check = kside_castling_check;
            else if (command == "O-O-O")
                check = qside_castling_check;
            else if (command[command.size()-2] == '=')
                check = promotion_check;
            else
                check = other_check;

            MoveList moves = Movegen(board).get_legal_moves();
            int i;
            for (i = 0; i < moves.size(); ++i) {
                if (!check(command, moves[i])) continue;
                to << static_cast<std::string>(moves[i]) << ' ';
                break;
            }

            if (i == moves.size()) {
                std::cerr << "\nTrouble in " + processed_file << std::endl;
                std::cerr << "Line: " << num_line << std::endl;
                std::cerr << "Move: " << counter/3 << ' ' << (counter % 3 == 2 ? "White" : "Black") << std::endl;
                throw std::runtime_error("");
            }
            board.make(moves[i]);
        }
        ++num_line;
        to << std::endl;
    }
    from.close();
    to.close();
}

void PGNParser::parse(const std::string &path) {
    try {
        hidden::first_processing(path);
        hidden::second_processing();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}