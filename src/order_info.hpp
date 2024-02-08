#ifndef ATENIKA_ORDER_INFO_HPP
#define ATENIKA_ORDER_INFO_HPP

#include "defs.hpp"
#include "move.hpp"

class OrderInfo {
private:
    int16_t m_ply;
    int32_t m_history[64][64]; // from square, to square;
    Move m_killers1[MAX_PLY], m_killers2[MAX_PLY];

public:
    static constexpr int32_t KILLER1_BONUS = 200'000;
    static constexpr int32_t KILLER2_BONUS = 100'000;

    OrderInfo();
    void operator++();
    void operator--();

    void add_killer(Move move);
    void add_history(uint8_t from, uint8_t to, int16_t depth);

    [[nodiscard]] Move get_killer1() const;
    [[nodiscard]] Move get_killer2() const;

    [[nodiscard]] int32_t get_history(uint8_t from, uint8_t to) const;
    [[nodiscard]] int16_t get_ply() const;
};

#endif //ATENIKA_ORDER_INFO_HPP
