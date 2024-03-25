//
// Created by timap on 06.03.2024.
//
#include "MinMax.h"

#include <algorithm>
#include <vector>

#include "../../Check/Check.h"
#include "../../PossibleMoves/PossibleMoves.h"
#include "../Evaluation/Evaluate.h"

//  {BLACK, WHITE}

/*
 * Maximizes value: White
 * Minimizes value: Black
 */

/**
 * @brief Array representing the best score a node can get for the current player \n
 * (i.e. infinites[Team::WHITE] is the best value for white).
 *
 * This array is used to represent the maximum and minimum possible evaluation values for the MinMax algorithm.
 * The first element is the minimum value (for the black player), and the second element is the maximum value (for the
 * white player).
 */
constexpr int infinites[2] = {INT32_MIN, INT32_MAX};

/**
 * @brief Implementation of the MinMax algorithm.
 *
 * This function recursively explores the game tree to a given depth and evaluates the board states.
 * It uses alpha-beta pruning to cut off branches that do not need to be explored.
 *
 * @param board The previous board state to be updated to the current one by move
 * @param move The move to update the previous board state to the current one
 * @param depth The depth to which the game tree should be explored
 * @param player The current player

 * @param alpha The best already explored option along the path to the root for the maximizer
 * @param beta The best already explored option along the path to the root for the minimizer
 * @param turn roughly the amount of half turns that have past
 * @return The evaluation value of the board state
 *
 * @note done like this with not using board.undoMove() and always copying the board because undoMove was
 * actually around 10% slower in the Perft test with O3 Optimization than just copying the whole board
 */
static int MinMax_impl(Board board, int depth, Team::Team player, Move move, int alpha, int beta, int turn) {
  // get the new board
  board.movePiece(move);

  Team::Team enemy = Team::getEnemyTeam(player);

  // return max value if the game has reached a terminal state
  if (Check::isChecked(board, enemy, board.positions.getKingPos(enemy))) return infinites[player];

  // otherwise if the search has reached the end with the depth left of 0 evaluate the board
  if (depth <= 0) return evaluateBoard(board, player);

  // get all moves
  std::vector<Move> move_list;
  PossibleMoves::getAllPossibleMoves(board, move_list, player);

  // go through all the moves
  if (player == Team::WHITE) {
    int maxEval = INT32_MIN;
    for (Move nextMove : move_list) {
      int eval = MinMax_impl(board, depth - 1, Team::BLACK, nextMove, alpha, beta, turn + 1);
      maxEval = std::max(maxEval, eval);

      alpha = std::max(alpha, eval);
      if (beta <= alpha) break;
    }
    return maxEval;

  } else {
    int minEval = INT32_MAX;
    for (Move nextMove : move_list) {
      int eval = MinMax_impl(board, depth - 1, Team::WHITE, nextMove, alpha, beta, turn + 1);
      minEval = std::min(minEval, eval);

      beta = std::min(beta, eval);
      if (beta <= alpha) break;
    }
    return minEval;
  }
}

// (function documentation is provided in the corresponding header)
SlimOptional<Move> MinMax(const Board& board, int depth, Team::Team player, int current_half_turn) {
  SlimOptional<Move> best_move = SlimOptional<Move>::nullopt();
  int alpha = INT32_MIN;
  int beta = INT32_MAX;

  // check if a terminal state has already been reached
  if (!board.positions.hasPiece(Team::WHITE, PieceType::KING) ||
      !board.positions.hasPiece(Team::BLACK, PieceType::KING))
    return SlimOptional<Move>::nullopt();

  // get all moves
  std::vector<Move> move_list;
  PossibleMoves::getAllPossibleMoves(board, move_list, player);

  // check if any moves directly result in a terminal state
  // to make sure that both kings still exist in the next depth
  // and no unexpected errors are created
  for (Move move : move_list) {
    if (board.grid[move.to].type == PieceType::KING) return SlimOptional(move);
  }

  // go through all the moves
  if (player == Team::WHITE) {
    int maxEval = INT32_MIN;
    for (Move nextMove : move_list) {
      int eval = MinMax_impl(board, depth - 1, Team::BLACK, nextMove, alpha, beta, current_half_turn + 1);
      if (eval > maxEval) {
        maxEval = eval;
        best_move = SlimOptional(nextMove);
      }

      alpha = std::max(alpha, eval);
      if (beta <= alpha) break;
    }

  } else {
    int minEval = INT32_MAX;
    for (Move nextMove : move_list) {
      int eval = MinMax_impl(board, depth - 1, Team::WHITE, nextMove, alpha, beta, current_half_turn + 1);
      if (eval < minEval) {
        minEval = eval;
        best_move = SlimOptional(nextMove);
      }

      beta = std::min(beta, eval);
      if (beta <= alpha) break;
    }
  }

  return best_move;
}
