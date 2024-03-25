//
// Created by timap on 06.03.2024.
//

#pragma once

#include "../../Board/Board.h"

/**
 * @brief Evaluates the current board.
 *
 * @param board The current board state
 * @param player The player who can currently make a move
 * @return The calculated score for a given board
 *
 * @note
 * positive value : white's advantage \n
 * negative value : black's advantage
 */
int evaluateBoard(const Board& board, Team::Team player);
