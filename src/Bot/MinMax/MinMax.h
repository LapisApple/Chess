//
// Created by timap on 06.03.2024.
//

#pragma once


#include "../../Board/Board.h"

/**
 * @brief wraps around and executes the MinMax algorithm to find the best move.
 *
 * @param board The current board state
 * @param depth The depth to which the game tree should be explored \n
 * (if \< 1 -\> function works as if depth = 1 e.g. one move ahead will still be tested)
 * @param player The team for which the best possible Move is searched for
 * @param currentTurn roughly the amount of half turns that have past
 * @return The best move possible for the specified player, or nullopt if no move is possible
 *
 * @note first checks if the board is actually legal meaning both kings exist in order to establish some invariance
 */
SlimOptional<Move> MinMax(const Board& board, int depth, Team::Team player, int currentTurn);