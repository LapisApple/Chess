//
// Created by timap on 17.03.2024.
//

#pragma once
#include "Board_impl//Board_8x8.h"
#include "Board_impl/Board_Positions.h"

/**
 * @brief checks that all Pieces in the grid are also in the Position Lists
 * @param grid the grid from which the Pieces for the comparison are taken
 * @param positions the board positions in which the grid pieces should be
 * @return true if all pieces in the grid are also in the position lists
 */
constexpr bool grid_in_positions(const Board_8x8& grid, const Board_Positions& positions) noexcept {
  for (int8_t team_id = 0; team_id < TEAM_AMOUNT; ++team_id) {
    Team::Team team = static_cast<Team::Team>(team_id);
    for (int8_t type_id = 1; type_id < PIECETYPE_AMOUNT; ++type_id) {
      PieceType::PieceType type = static_cast<PieceType::PieceType>(type_id);
      Board_Positions::PositionsSlice current_pos_list = positions.getVec(team, type);

      for (int i = 0; i < current_pos_list.size; ++i) {
        int8_t current_pos = current_pos_list[i];

        Piece grid_piece = grid[current_pos];
        const bool not_same = grid_piece.team != team || grid_piece.type != type;
        if (not_same) return false;
      }
    }
  }
  return true;
}

/**
 * @brief checks that all Pieces in the Position Lists are also in the Grid
 * @param grid the board positions from which the Pieces for the comparison are taken
 * @param positions the grid in which the board positions pieces should be
 * @return true if all pieces in the board positions are also in the grid
 */
constexpr bool positions_in_grid(const Board_8x8& grid, const Board_Positions& positions) noexcept {
  for (int8_t grid_pos = 0; grid_pos < BOARD_SIZE; ++grid_pos) {
    Piece grid_piece = grid[grid_pos];
    if (grid_piece.team == Team::NONE) continue;

    Board_Positions::PositionsSlice current_pos_list = positions.getVec(grid_piece.team, grid_piece.type);

    bool exists = false;
    for (int j = 0; j < current_pos_list.size; ++j) {
      int8_t list_pos = current_pos_list[j];

      if (list_pos == grid_pos) {
        exists = true;
        break;
      }
    }
    if (!exists) return false;
  }
  return true;
}

/**
 * @brief checks whether both representations of the Board are equal
 * @param grid the grid as one side of the comparison
 * @param positions the Board_Positions as the other side of the comparison
 * @return true if both representations of the Board are equal
 */
constexpr bool positions_equals_grid(const Board_8x8& grid, const Board_Positions& positions) noexcept {
  /* basically the idea is to think of the board as a set of (team, type, position)
   * and then test that both are subsets? of each other meaning they are the same
   */
  return positions_in_grid(grid, positions) && grid_in_positions(grid, positions);
}
