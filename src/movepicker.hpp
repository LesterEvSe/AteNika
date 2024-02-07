#ifndef ATENIKA_MOVEPICKER_HPP
#define ATENIKA_MOVEPICKER_HPP

#include "move_list.hpp"
#include "order_info.hpp"

class MovePicker {
private:
    MoveList &m_move_list;
    uint8_t m_curr_node;

public:
    explicit MovePicker(MoveList *move_list, OrderInfo &order_info);
    [[nodiscard]] bool has_next() const;
    const Move &get_next(); // Using selection sorting in place, O(n) at a time
};

#endif //ATENIKA_MOVEPICKER_HPP
