// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#include "core/move_list.hpp"

uint8_t MoveList::size() const { return m_size; }

Move &MoveList::operator[](uint8_t ind) { return m_moves[ind]; }

void MoveList::emplace_back(const Move &move) { m_moves[m_size++] = move; }

int32_t MoveList::get_score(uint8_t ind) const { return m_scores[ind]; };

void MoveList::set_score(uint8_t ind, int32_t val) { m_scores[ind] = val; }