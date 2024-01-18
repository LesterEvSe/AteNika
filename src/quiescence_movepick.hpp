#ifndef ATENIKA_QUIESCENCE_MOVEPICK_HPP
#define ATENIKA_QUIESCENCE_MOVEPICK_HPP

#include "move_list.hpp"

class QMovePicker {
private:
    MoveList &m_move_list;
    uint8_t m_curr_node; // First unpicked move
    uint8_t m_size;

public:
    explicit QMovePicker(MoveList *move_list);
    [[nodiscard]] bool has_next() const;
    const Move &get_next();
};

#endif //ATENIKA_QUIESCENCE_MOVEPICK_HPP
