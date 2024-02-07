#ifndef ATENIKA_ORDER_INFO_HPP
#define ATENIKA_ORDER_INFO_HPP

#include "defs.hpp"
#include "move.hpp"

class OrderInfo {
private:
    int16_t m_ply;
    Move m_killers1[DRAW_RULE_50], m_killers2[DRAW_RULE_50];

public:
    static constexpr int32_t KILLER1_BONUS = 200;
    static constexpr int32_t KILLER2_BONUS = 100;

    OrderInfo();
    void operator++();
    void operator--();

    void add_killer(Move move);
    [[nodiscard]] Move get_killer1() const;
    [[nodiscard]] Move get_killer2() const;
    [[nodiscard]] int16_t get_ply() const;
};

#endif //ATENIKA_ORDER_INFO_HPP
