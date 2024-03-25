//
// Created by timap on 13.03.2024.
//

#pragma once
#include <vector>

#include "../Board/Board.h"
#include "../PossibleMoves/PossibleMoves.h"

namespace CheckMate {
/**
 * @brief Checks if a player is checkmated
 *
 * @param board The current state of the chess board
 * @param player The player to check for checkmate (Team::WHITE or Team::BLACK)
 * @return true if the player is checkmated, otherwise false
 */
inline bool isCheckMated(const Board& board, Team::Team player) noexcept {
  std::vector<Move> move_list;
  PossibleMoves::getAllPossibleMoves(board, move_list, player);
  PossibleMoves::trimMovesPuttingPlayerIntoCheckmate(board, move_list, player);

  const bool no_possible_moves_left = move_list.empty();
  return no_possible_moves_left;
}
}  // namespace CheckMate
