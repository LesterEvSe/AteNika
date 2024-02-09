#ifndef ATENIKA_QMOVEPICKER_HPP
#define ATENIKA_QMOVEPICKER_HPP

#include "move_list.hpp"
#include "zobrist_hash.hpp"

class QMovePicker {
private:
    MoveList &m_move_list;
    uint8_t m_curr_node;
    uint8_t m_size;

public:
    explicit QMovePicker(MoveList *move_list, const ZobristHash &hash);
    [[nodiscard]] bool has_next() const;
    const Move &get_next();
};

#endif //ATENIKA_QMOVEPICKER_HPP
