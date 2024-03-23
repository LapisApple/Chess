//
// Created by timap on 27.01.2024.
//

#pragma once

#include <cstdint>
#include "../Types/BasicChessTypes.h"
#include "../Board/Board.h"

namespace Check {
    /**
     * @brief Checks if a player is in check
     *
     * @param board The current state of the chess board
     * @param player The player to check for being in Check (Team::WHITE or Team::BLACK)
     * @param the position of the players king
     * @return true if the player is in Check, otherwise false
     */
    bool isChecked(const Board& board, Team::Team team, int8_t king_pos) noexcept;
}