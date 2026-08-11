// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include <gtest/gtest.h>

#include "bitboard/bitfunc.hpp"

TEST(BitfuncTest, get_cell_func_and_FIELD) {
  uint8_t actual = get_cell(FIELD[51]);
  ASSERT_EQ(51, actual);
}

TEST(BitfuncTest, get_cell_func) {
  std::string actual = "b6";
  ASSERT_EQ(41, get_cell(actual));
}