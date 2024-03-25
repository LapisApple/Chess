//
// Created by timap on 14.03.2024.
//

#pragma once

#include <string>

#include "../../Board/Board_impl//Board_8x8.h"
#include "../../Board/Board_impl/Board_Extra.h"
#include "../../Types/Move/Move.h"

namespace Print {

/**
 * @brief Converts a board position to a chess notation string
 *
 * @param pos The board position to convert to a string (0=..\<64)
 * @return a string with the the position chess notation
 *
 * @note 0 represents the top left corner of the board (a8 in chess notation)
 * and 63 represents the bottom right corner (h1 in chess notation).
 */
std::string position_to_string(int8_t pos);

/**
 * @brief Converts a Move object to a string representation
 *
 * @param move The Move object to convert to a string
 * @param is_capture A boolean indicating whether the move results in a capture
 * @return A string representation of the move in the same notation as used in the exercises
 *
 * @example
 * normal move: Rh1f1, \n
 * capture: Pe4xf5, \n
 * promotion : Pa7a8=Q \n
 * castling: Ke1c1 \n
 */
[[nodiscard]] std::string move_as_string(Move move, bool is_capture);

/**
 * @brief Prints the current state of the chess board to an output stream
 *
 * @param board_grid The current state of the chess board
 * @param os The output stream to which to print the board
 */
void print_board_grid_state(const Board_8x8& board_grid, std::ostream& os);

// Board_Extra extra not a reverence because of small size (2b)
/**
 * @brief Converts the current state of the chess board to Forsyth-Edwards Notation (FEN)
 *
 * @param grid The current state of the chess board
 * @param extra Additional information about the board state (e.g., castling rights, en passant square)
 * @param current_team The team that is currently to move (either Team::WHITE or Team::BLACK)
 * @param current_turn The current turn number
 * @param current_half_turn The number of half-turns since the last pawn move or capture
 *
 * @return A string representation of the current board state in Forsyth-Edwards Notation
 */
std::string board_state_to_fen(const Board_8x8& grid, Board_Extra extra, Team::Team current_team, int current_turn,
                               int current_half_turn);
}  // namespace Print
