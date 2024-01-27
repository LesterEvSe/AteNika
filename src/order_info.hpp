#ifndef ATENIKA_ORDER_INFO_HPP
#define ATENIKA_ORDER_INFO_HPP

#include "defs.hpp"
#include "move.hpp"

class OrderInfo {
private:
    int32_t m_history[COLOR_SIZE][64][64];
    Move m_killer1[50], m_killer2[50];
    uint8_t m_ply;

public:
    static constexpr int32_t KILLER1_BONUS = 2000;
    static constexpr int32_t KILLER2_BONUS = 1000;

    OrderInfo();
    void operator++();
    void operator--();

    void update_killers(Move move);
    void inc_history(Color color, uint8_t from, uint8_t to, int16_t depth);

    [[nodiscard]] Move get_killer1() const;
    [[nodiscard]] Move get_killer2() const;
    [[nodiscard]] int32_t get_history(Color color, uint8_t from, uint8_t to) const;
};

#endif //ATENIKA_ORDER_INFO_HPP
