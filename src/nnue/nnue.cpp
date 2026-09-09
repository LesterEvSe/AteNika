// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include "nnue/nnue.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <vector>

#ifdef __AVX2__
#define ATENIKA_AVX2
#include <immintrin.h>
#endif

#include "bitboard/bitfunc.hpp"
#include "core/board.hpp"

// Defined by the file CMake generates from ATENIKA_NET.
extern const unsigned char ATENIKA_NET[];

namespace {
  using namespace NNUE;
  using namespace NNUE::detail;

  constexpr size_t NET_PAYLOAD = sizeof(int16_t) * (INPUT * HIDDEN + HIDDEN + 2 * HIDDEN + 1);
  constexpr size_t NET_PADDING = (64 - NET_PAYLOAD % 64) % 64;

  // alignas(64) mirrors bullet's `#[repr(C, align(64))]` accumulator.
  struct alignas(64) Network {
    int16_t feature_weights[INPUT][HIDDEN];
    int16_t feature_biases[HIDDEN];
    int16_t output_weights[2 * HIDDEN];
    int16_t output_bias;

    // Must be set up explicity, otherwise MSVC warning C4324 from CI.
    uint8_t padding[NET_PADDING];
  };

  static_assert(ATENIKA_NET_SIZE == sizeof(Network),
                "the embedded net does not match the layout above");

  Network &net() {
    static Network instance = [] {
      Network fresh;
      std::memcpy(&fresh, ATENIKA_NET, sizeof(fresh));
      return fresh;
    }();

    return instance;
  }

  int32_t scale(int64_t dot) {
    dot /= QA;
    dot += net().output_bias;
    return static_cast<int32_t>(dot * SCALE / (QA * QB));
  }


  // Rejects a net whose output weights could overflow the SIMD path.
  bool weights_fit_simd(const Network &candidate) {
    int64_t lane[8] = {};

    for (size_t i = 0; i < 2 * HIDDEN; ++i) {
      const int64_t magnitude = std::abs(static_cast<int64_t>(candidate.output_weights[i]));

      if (QA * magnitude > INT16_MAX)
        return false;
      lane[(i % 16) / 2] += magnitude;
    }

    for (const int64_t total : lane)
      if (QA * QA * total > INT32_MAX)
        return false;

    return true;
  }

#ifdef ATENIKA_AVX2
  static_assert(HIDDEN % 16 == 0, "the AVX2 paths step sixteen lanes at a time");

  // SCReLU against one perspective's weights, added into `lanes`.
  void screlu_dot(const int16_t *values, const int16_t *weights, __m256i &lanes) {
    const __m256i floor = _mm256_setzero_si256();
    const __m256i ceiling = _mm256_set1_epi16(static_cast<int16_t>(QA));

    for (int i = 0; i < HIDDEN; i += 16) {
      __m256i activation = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(values + i));
      activation = _mm256_min_epi16(_mm256_max_epi16(activation, floor), ceiling);

      const __m256i weight = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(weights + i));

      // madd pairs the lanes: (a*w)*a summed two at a time into int32.
      lanes = _mm256_add_epi32(
          lanes, _mm256_madd_epi16(activation, _mm256_mullo_epi16(activation, weight)));
    }
  }

  int64_t horizontal_sum(__m256i lanes) {
    __m256i wide = _mm256_add_epi64(_mm256_cvtepi32_epi64(_mm256_castsi256_si128(lanes)),
                                    _mm256_cvtepi32_epi64(_mm256_extracti128_si256(lanes, 1)));

    __m128i half =
        _mm_add_epi64(_mm256_castsi256_si128(wide), _mm256_extracti128_si256(wide, 1));
    half = _mm_add_epi64(half, _mm_unpackhi_epi64(half, half));

    return static_cast<int64_t>(_mm_cvtsi128_si64(half));
  }
#endif

  // target = source - removed columns + added columns, in one pass over the
  // accumulator instead of a copy followed by one pass per column.
  template <int REMOVED, int ADDED>
  void fuse(const int16_t *source, const int16_t *const *removed, const int16_t *const *added,
            int16_t *target) {
#ifdef ATENIKA_AVX2
    for (int i = 0; i < HIDDEN; i += 16) {
      __m256i value = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(source + i));

      for (int f = 0; f < REMOVED; ++f)
        value = _mm256_sub_epi16(
            value, _mm256_loadu_si256(reinterpret_cast<const __m256i *>(removed[f] + i)));

      for (int f = 0; f < ADDED; ++f)
        value = _mm256_add_epi16(
            value, _mm256_loadu_si256(reinterpret_cast<const __m256i *>(added[f] + i)));

      _mm256_storeu_si256(reinterpret_cast<__m256i *>(target + i), value);
    }
#else
    for (int i = 0; i < HIDDEN; ++i) {
      int16_t value = source[i];

      for (int f = 0; f < REMOVED; ++f)
        value = static_cast<int16_t>(value - removed[f][i]);

      for (int f = 0; f < ADDED; ++f)
        value = static_cast<int16_t>(value + added[f][i]);

      target[i] = value;
    }
#endif
  }

  const int16_t *column(Color perspective, const Feature &feature) {
    return net().feature_weights[feature_index(perspective, feature.color, feature.piece,
                                               feature.cell)];
  }

  // child = parent - removed columns + added columns.
  void apply(const Accumulator &parent, const Delta &delta, Accumulator &child) {
    for (const Color perspective : {BLACK, WHITE}) {
      const int16_t *removed[2];
      const int16_t *added[2];

      for (uint8_t f = 0; f < delta.removed_count; ++f)
        removed[f] = column(perspective, delta.removed[f]);

      for (uint8_t f = 0; f < delta.added_count; ++f)
        added[f] = column(perspective, delta.added[f]);

      const int16_t *source = parent.values[perspective];
      int16_t *target = child.values[perspective];

      // A move removes at most two columns and adds at most two, so the counts
      // are known constants inside each branch and the loops above unroll away.
      switch (delta.removed_count * 3 + delta.added_count) {
      case 0: fuse<0, 0>(source, removed, added, target); break;
      case 1: fuse<0, 1>(source, removed, added, target); break;
      case 2: fuse<0, 2>(source, removed, added, target); break;
      case 3: fuse<1, 0>(source, removed, added, target); break;
      case 4: fuse<1, 1>(source, removed, added, target); break;
      case 5: fuse<1, 2>(source, removed, added, target); break;
      case 6: fuse<2, 0>(source, removed, added, target); break;
      case 7: fuse<2, 1>(source, removed, added, target); break;
      default: fuse<2, 2>(source, removed, added, target); break;
      }
    }
  }
} // namespace

NNUE::AccumulatorStack::AccumulatorStack()
    : m_entries(std::make_unique_for_overwrite<Entry[]>(MAX_PLIES)), m_top(0) {
  m_entries[0].computed = false;
}

NNUE::AccumulatorStack::AccumulatorStack(const AccumulatorStack &other) : AccumulatorStack() {
  invalidate(other.m_top);
}

NNUE::AccumulatorStack &NNUE::AccumulatorStack::operator=(const AccumulatorStack &other) {
  if (this != &other)
    invalidate(other.m_top);
  return *this;
}

void NNUE::AccumulatorStack::invalidate(int32_t top) {
  m_top = top;
  for (int32_t i = 0; i <= m_top; ++i)
    m_entries[i].computed = false;
}

void NNUE::AccumulatorStack::push(const Delta &delta) {
  m_entries[++m_top].delta = delta;
  m_entries[m_top].computed = false;
}

void NNUE::AccumulatorStack::pop() { --m_top; }

const NNUE::Accumulator &NNUE::AccumulatorStack::top(const Board &board) {
  if (m_entries[m_top].computed)
    return m_entries[m_top].acc;

  // The nearest ancestor whose parent is already known.
  int32_t first = m_top;
  while (first > 0 && !m_entries[first - 1].computed)
    --first;

  if (first == 0) {
    refresh(board, m_entries[m_top].acc);
    m_entries[m_top].computed = true;
    return m_entries[m_top].acc;
  }

  for (int32_t i = first; i <= m_top; ++i) {
    apply(m_entries[i - 1].acc, m_entries[i].delta, m_entries[i].acc);
    m_entries[i].computed = true;
  }

  return m_entries[m_top].acc;
}

bool NNUE::load(const std::string &path) {
  std::ifstream in(path, std::ios::binary);
  if (!in)
    return false;

  const std::vector<char> buffer((std::istreambuf_iterator<char>(in)),
                                 std::istreambuf_iterator<char>());
  if (buffer.size() != sizeof(Network))
    return false;

  Network candidate;
  std::memcpy(&candidate, buffer.data(), sizeof(candidate));

  if (!weights_fit_simd(candidate))
    return false;

  net() = candidate;
  return true;
}

bool NNUE::detail::net_fits_simd() { return weights_fit_simd(net()); }

uint16_t NNUE::detail::feature_index(Color perspective, Color color, PieceType piece,
                                     uint8_t cell) {
  const uint16_t relative_color = color == perspective ? 0 : 1;
  const uint8_t relative_cell = perspective == WHITE ? cell : cell ^ 56;
  return relative_color * 384 + static_cast<uint16_t>(piece) * 64 + relative_cell;
}

void NNUE::refresh(const Board &board, Accumulator &acc) {
  for (const Color perspective : {BLACK, WHITE})
    std::copy_n(net().feature_biases, HIDDEN, acc.values[perspective]);

  for (const Color color : {BLACK, WHITE})
    for (const PieceType piece : PIECES) {
      bitboard pieces = board.get_pieces(color, piece);

      while (pieces) {
        const uint8_t cell = pop_lsb(pieces);

        for (const Color perspective : {BLACK, WHITE}) {
          const int16_t *column =
              net().feature_weights[feature_index(perspective, color, piece, cell)];

          for (int i = 0; i < HIDDEN; ++i)
            acc.values[perspective][i] =
                static_cast<int16_t>(acc.values[perspective][i] + column[i]);
        }
      }
    }
}

int32_t NNUE::detail::forward_scalar(const Accumulator &acc, Color side_to_move) {
  const Color them = side_to_move == WHITE ? BLACK : WHITE;

  // int64 to prevent overflow.
  int64_t sum = 0;

  for (int i = 0; i < HIDDEN; ++i) {
    const int64_t us = std::clamp<int32_t>(acc.values[side_to_move][i], 0, QA);
    const int64_t opponent = std::clamp<int32_t>(acc.values[them][i], 0, QA);

    sum += us * us * net().output_weights[i];
    sum += opponent * opponent * net().output_weights[HIDDEN + i];
  }

  return scale(sum);
}

int32_t NNUE::detail::forward(const Accumulator &acc, Color side_to_move) {
#ifdef ATENIKA_AVX2
  const Color them = side_to_move == WHITE ? BLACK : WHITE;

  __m256i lanes = _mm256_setzero_si256();
  screlu_dot(acc.values[side_to_move], net().output_weights, lanes);
  screlu_dot(acc.values[them], net().output_weights + HIDDEN, lanes);

  return scale(horizontal_sum(lanes));
#else
  return forward_scalar(acc, side_to_move);
#endif
}

int32_t NNUE::evaluate(const Board &board) {
  return detail::forward(board.get_accumulator(), board.get_curr_move());
}
