#ifndef ATENIKA_BITFUNC_HPP
#define ATENIKA_BITFUNC_HPP

#include <bit>
#include <iostream> // for std::cerr
#include <string>
#include <string_view>

#include "defs.hpp"

[[nodiscard]] inline uint8_t get_rank(uint8_t cell) { return cell >> 3; }
[[nodiscard]] inline uint8_t get_file(uint8_t cell) { return cell & 7; }
[[nodiscard]] inline Color get_opposite_move(Color color) { return color == WHITE ? BLACK : WHITE; }

// -march=x86-64-v3 guarantees POPCNT, BMI1 and LZCNT, so each of these is a
// single instruction.
[[nodiscard]] inline uint8_t count_bits(bitboard bb) {
  return static_cast<uint8_t>(std::popcount(bb));
}

inline void set(bitboard &bb, uint8_t cell) { bb |= ONE << cell; }
inline void reset(bitboard &bb, uint8_t cell) { bb &= ~(ONE << cell); }

[[nodiscard]] inline uint8_t lsb(bitboard bb) { return static_cast<uint8_t>(std::countr_zero(bb)); }
[[nodiscard]] inline uint8_t msb(bitboard bb) {
  return static_cast<uint8_t>(63 - std::countl_zero(bb));
}

inline uint8_t pop_lsb(bitboard &bb) {
  uint8_t pos = lsb(bb);
  bb &= bb - 1;
  return pos;
}

[[nodiscard]] inline uint8_t get_cell(std::string_view notation) {
  uint8_t file = notation[0] - 'a';
  uint8_t rank = notation[1] - '1';
  return (rank << 3) + file;
}

inline void error(const std::string &msg) {
  std::cerr << msg;
  exit(1);
}

// For debugging action
inline void printbb(bitboard bb) {
  std::cout << "   ";

  for (char let = 'A'; let <= 'H'; ++let)
    std::cout << ' ' << let;
  std::cout << '\n';

  for (int8_t i = 7; i >= 0; --i) {
    std::cout << i + 1 << " |";
    for (uint8_t j = 0; j < 8; ++j)
      std::cout << ' ' << bool(bb & (ONE << (i * 8 + j)));
    std::cout << '\n';
  }
  std::cout << "\n--------------------\n\n";
}

#endif // ATENIKA_BITFUNC_HPP
