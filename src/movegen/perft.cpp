#include "movegen/perft.hpp"

#include <iostream>

#include "movegen/movegen.hpp"

int64_t Perft::run(Board &board, int depth) {
  if (depth < 1)
    return 1;

  MoveList move_list = Movegen(board).get_legal_moves();

  // Bulk counting: at depth 1 the number of legal moves is the answer,
  // so there is no need to make and unmake each of them.
  if (depth-- == 1)
    return move_list.size();

  int64_t nodes = 0;
  for (uint8_t i = 0; i < move_list.size(); ++i) {
    board.make(move_list[i]);
    nodes += run(board, depth);
    board.unmake(move_list[i]);
  }
  return nodes;
}

int64_t Perft::divide(Board &board, int depth) {
  if (depth < 1)
    return 1;

  MoveList move_list = Movegen(board).get_legal_moves();
  int64_t total = 0;

  for (uint8_t i = 0; i < move_list.size(); ++i) {
    board.make(move_list[i]);
    // The root move itself is the leaf when depth is 1.
    int64_t nodes = depth == 1 ? 1 : run(board, depth - 1);
    board.unmake(move_list[i]);

    std::cout << static_cast<std::string>(move_list[i]) << ": " << nodes << std::endl;
    total += nodes;
  }

  std::cout << "\nNodes searched: " << total << std::endl;
  return total;
}
