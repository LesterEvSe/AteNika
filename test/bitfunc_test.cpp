#include <gtest/gtest.h>
#include "bitfunc.hpp"

TEST(Bitfunc, get_cell_func_and_FIELD) {
    uint8_t actual = get_cell(FIELD[51]);
    ASSERT_EQ(51, actual);
}

TEST(Bitfunc, get_cell_func) {
    std::string actual = "b6";
    ASSERT_EQ(41, get_cell(actual));
}