#ifndef ATENICA_MOVEPICKER_HPP
#define ATENICA_MOVEPICKER_HPP

#include "move_list.hpp"

class MovePicker {
private:
    MoveList &m_move_list;
    uint8_t m_curr_node; // First unpicked move

public:
    explicit MovePicker(MoveList *move_list);
    [[nodiscard]] bool has_next() const;
    const Move &get_next(); // Using selection sorting in place, O(n) at a time
};

#endif //ATENICA_MOVEPICKER_HPP
