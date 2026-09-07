// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include <ranges>
#include <sstream>
#include <string>
#include <vector>

namespace FenMirror {
  inline char swap_case(char symbol) {
    if (symbol >= 'a' && symbol <= 'z')
      return static_cast<char>(symbol - 'a' + 'A');
    if (symbol >= 'A' && symbol <= 'Z')
      return static_cast<char>(symbol - 'A' + 'a');
    return symbol;
  }

  inline std::string mirror(const std::string &fen) {
    std::istringstream fen_stream(fen);
    std::string placement;
    std::string side;
    std::string castling;
    std::string en_passant;
    std::string ply;
    std::string moves;
    fen_stream >> placement >> side >> castling >> en_passant >> ply >> moves;

    std::vector<std::string> ranks;
    std::string rank;
    std::istringstream placement_stream(placement);
    while (std::getline(placement_stream, rank, '/'))
      ranks.push_back(rank);

    std::string flipped;
    for (const std::string &flipped_rank : std::views::reverse(ranks)) {
      if (!flipped.empty())
        flipped += '/';
      for (const char symbol : flipped_rank)
        flipped += swap_case(symbol);
    }

    std::string swapped_castling;
    for (const char right : {'K', 'Q', 'k', 'q'})
      if (castling.find(swap_case(right)) != std::string::npos)
        swapped_castling += right;
    if (swapped_castling.empty())
      swapped_castling = "-";

    if (en_passant != "-")
      en_passant[1] = static_cast<char>('0' + 9 - (en_passant[1] - '0'));

    return flipped + ' ' + (side == "w" ? "b" : "w") + ' ' + swapped_castling + ' ' + en_passant +
           ' ' + ply + ' ' + moves;
  }

  // The CPW perft set, which perft_test.cpp and bench.cpp already trust, plus
  // both sides to move and a pawnless endgame.
  inline const std::vector<std::string> POSITIONS = {
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1",
      "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
      "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
      "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
      "4k3/p7/8/8/8/8/1PP5/R3K3 w Q - 0 1",
      "4k3/8/8/8/8/8/8/2B1KB2 w - - 0 1",
      "8/8/4k3/8/8/4K3/8/6Q1 b - - 0 1",
  };
} // namespace FenMirror
