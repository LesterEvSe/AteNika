#include <gtest/gtest.h>
#include "rays.hpp"
#include "attacks.hpp"

#include <random>
#include <chrono>

class PerftComparison : public testing::Test
{
public:
    static void SetUpTestCase() {
        init_bits_pre_calculation();
        Rays::init();
        Attacks::init();
    }
};

constexpr uint16_t mask = 0xFFFF;
uint8_t memo[1 << 16];

void init() {
    for (uint32_t i = 0; i < (1 << 16); ++i)
        memo[i] = memo[i >> 1] + (i & 1);
}

uint8_t count_bits_my_func(uint64_t num) {
    uint8_t sum = 0;
    for (uint8_t i = 0; i < 4; ++i) {
        sum += memo[num & mask];
        num >>= 16;
    }
    return sum;
}

uint8_t count_bits_popcount(uint64_t num) {
    return std::popcount(num);
}

void check(uint8_t (*bits_count)(uint64_t), const std::vector<uint64_t> &nums) {
    for (size_t i = 0; i < nums.size(); ++i)
        bits_count(nums[i]);

//    for (uint64_t i = 0; i < 50; ++i)
//        std::cout << (int)bits_count(nums[i]) << ' ';
//    std::cout << std::endl;
}

TEST_F(PerftComparison, count_bits_func) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist(0, std::numeric_limits<uint64_t>::max());

    std::vector<uint64_t> nums(100'000'000);
    for (uint64_t i = 0; i < nums.size(); ++i)
        nums[i] = dist(gen);

    auto start_time = std::chrono::steady_clock::now();
    check(count_bits_popcount, nums);
    auto end_time1 = std::chrono::steady_clock::now();
    check(count_bits_my_func, nums);
    auto end_time2 = std::chrono::steady_clock::now();
    check(count_bits, nums);
    auto end_time3 = std::chrono::steady_clock::now();

    auto duration_popcount = std::chrono::duration_cast<std::chrono::milliseconds>(end_time1 - start_time);
    auto duration_my_func = std::chrono::duration_cast<std::chrono::milliseconds>(end_time2 - end_time1);
    auto duration_my_func_better = std::chrono::duration_cast<std::chrono::milliseconds>(end_time3 - end_time2);

    std::cout << "popcount: " << duration_popcount << std::endl;
    std::cout << "my_count (old): " << duration_my_func << std::endl;
    std::cout << "my_count (new): " << duration_my_func_better << std::endl;
}