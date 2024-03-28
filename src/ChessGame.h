//
// Created by timap on 13.03.2024.
//

#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Board/Board.h"
#include "Bot/Evaluation/Evaluate.h"
#include "Bot/MinMax/MinMax.h"
#include "Check/CheckMate.h"
#include "IO/Parser/FenParsing.h"
#include "IO/Print/Print.h"
#include "PossibleMoves/PossibleMoves.h"
#include "Types/Move/UserMove.h"

/**
 * @brief Enum class for the result of a Method Call.
 *
 * @note It can either be SUCCESS if the method worked as expected,
 * or FAILURE if it was not able to do it.
 */
enum class Result : int8_t { SUCCESS, FAILURE };

/**
 * @brief Class for a chess game.
 *
 * Wraps the Chess Board and other Functionality into a simple to use ChessGame class
 */
class ChessGame {
  /**
   * @brief Struct for the History of the current Chess Game.
   */
  struct History {
    /**
     * @brief the executed move
     */
    ExecutedMove move = ExecutedMove();
    /**
     * @brief the number of half moves since last pawn move or capture before the move was executed
     */
    int half_turn_before_move = 0;
  };

  /**
   * @brief the Chess Board that store Piece Location and other data about the board
   */
  Board board;
  /**
   * @brief the current Player, who can currently make a move)
   */
  Team::Team current_move_maker;
  /**
   * @brief the current Turn number (incremented after each time black moves)
   */
  int current_turn;
  /**
   * @brief the current number of half moves since last pawn move or capture
   */
  int half_moves;
  /**
   * @brief all previous game state's as a stack of executed moves half moves
   */
  std::vector<History> history;
  /**
   * @brief the team of the bot
   */
  Team::Team bot_color;

  /**
   * @brief Executes a move without checking for its legality.
   *
   * @param move The move to be executed
   *
   * @note save the move to the history to be able to undo it
   */
  void executeMove(Move move) {
    // create history
    History current_history = History();
    current_history.half_turn_before_move = half_moves;
    // make move on board
    current_history.move = board.movePiece(move);

    // update other values
    // turn
    if (current_move_maker == Team::BLACK) current_turn++;
    // player to move
    current_move_maker = Team::getEnemyTeam(current_move_maker);
    // 50 move rule
    if (move.piece.type == PieceType::PAWN || board.isMoveCapture(move))
      half_moves = 0;
    else
      half_moves++;

    // save previous State
    history.push_back(current_history);
  }

 public:
  // constructor
  /**
   * @brief constructs the Starting Chess Game State.
   */
  inline ChessGame()
      : board(Board()), current_move_maker(Team::WHITE), current_turn(1), half_moves(0), bot_color(Team::BLACK) {}

  /**
   * @brief constructs a new game from a preProcessed FEN notated String.
   *
   * @param fen The preprocessed Fen object representing a Chess state in Fen Notation
   * @param bot_team the team that the bot will be representing
   */
  explicit inline ChessGame(const Fen& fen, Team::Team bot_team)
      : board(Board(fen)),
        current_move_maker(fen.current_player),
        current_turn(fen.current_turn),
        half_moves(fen.amount_half_moves),
        bot_color(bot_team) {}

  // make moves

  /**
   * @brief Executes a Player made Move when the given move is valid.
   *
   * @param move The move to be made
   * @return The result of whether the move was executed or not
   */
  Result executePlayerMove(UserMove move) {
    std::vector<Move> move_list;
    PossibleMoves::getAllPossibleMoves(board, move_list, current_move_maker);
    PossibleMoves::trimMovesPuttingPlayerIntoCheckmate(board, move_list, current_move_maker);

    for (Move& current_move : move_list) {
      if (move.equals(current_move, board.isMoveCapture(current_move))) {
        executeMove(current_move);
        return Result::SUCCESS;
      }
    }
    return Result::FAILURE;
  }

  /**
   * @brief Lets the bot make a move.
   *
   * @return The result of whether the move was executed or not
   */
  Result let_bot_make_move() {
    SlimOptional<Move> move = getBestMove();
    if (!move.has_value()) return Result::FAILURE;
    executeMove(move.data);
    return Result::SUCCESS;
  }

  // undo moves
  /**
   * @brief Undoes the previous move.
   *
   * @return The result of whether there were any moves that could be undone or not
   */
  Result undo_move() {
    // check that there are moves that can be undone
    if (history.empty()) return Result::FAILURE;

    const History& last_elem = history.back();

    board.undoMove(last_elem.move);
    half_moves = last_elem.half_turn_before_move;

    if (current_move_maker == Team::WHITE) current_turn--;
    current_move_maker = Team::getEnemyTeam(current_move_maker);

    history.pop_back();
    return Result::SUCCESS;
  }

  // print
  /**
   * @brief Prints the current board to the given output stream
   *
   * @param os The output stream to print the board to
   */
  inline void print_current_board(std::ostream& os) const {
    const std::string_view team = (current_move_maker == Team::WHITE) ? "White" : "Black";
    const std::string_view bot = (current_move_maker == bot_color) ? "Bot" : "Player";
    os << "Current Turn: " << team << " (" << bot << ")\n";
    Print::print_board_grid_state(board.grid, os);
    os << std::endl;
  }

  /**
   * @brief Converts the current board state to a Fen string
   *
   * @return A FEN notated String of the current chess game state
   */
  [[nodiscard]] inline std::string asFen() const {
    return Print::board_state_to_fen(board.grid, board.extra, current_move_maker, current_turn, half_moves);
  }

  /**
   * @brief Prints all moves possible for the current player to the given output stream
   *
   * @param os The output stream to be printed to
   * @param amount_per_row The amount of moves per row
   */
  void print_all_possible_moves(std::ostream& os, uint64_t amount_per_row) const {
    std::vector<Move> move_list;
    PossibleMoves::getAllPossibleMoves(board, move_list, current_move_maker);
    PossibleMoves::trimMovesPuttingPlayerIntoCheckmate(board, move_list, current_move_maker);

    const uint64_t highest_num_in_row = amount_per_row - 1;
    for (uint64_t i = 0; i < move_list.size(); ++i) {
      Move& move = move_list[i];
      os << Print::move_as_string(move, board.isMoveCapture(move));
      os << ((i % amount_per_row == highest_num_in_row) ? '\n' : ' ');
    }
    os << std::endl;
  }

  /**
   * @brief Prints a move to the given output stream
   *
   * @param os The output stream to be printed to
   * @param move The move to be printed
   */
  void printMove(std::ostream& os, const Move& move) {
    os << Print::move_as_string(move, board.isMoveCapture(move)) << std::endl;
  }

  // helper Methods
  /**
   * @brief Calculates the best move for the current player using MinMax with a search depth of 5.
   *
   * @return The best move currently, or no move if the player is in checkMate
   */
  [[nodiscard]] inline SlimOptional<Move> getBestMove() const {
    const int amount_half_turns_since_start = current_turn * 2 + (current_move_maker == Team::BLACK);
    return MinMax(board, 5, current_move_maker, amount_half_turns_since_start);
  }

  /**
   * @brief Checks if the one, who can currently mak a move, is in check
   *
   * @return True if the one, who can currently mak a move, is in check, false otherwise
   */
  [[nodiscard]] inline bool isCurrentMoveMakerInCheck() const {
    const bool is_checked = Check::isChecked(board, current_move_maker, board.positions.getKingPos(current_move_maker));
    return is_checked;
  }

  /**
   * @brief Checks if the one, who can currently mak a move, is in checkmate
   *
   * @return True if the one, who can currently mak a move, is in checkmate, false otherwise
   */
  [[nodiscard]] inline bool isCurrentMoveMakerCheckmated() const {
    const bool is_checkmated = CheckMate::isCheckMated(board, current_move_maker);
    return is_checkmated;
  }

  /**
   * @brief Calculates the Score (for White) for the current board state.
   *
   * @return The evaluation score for the white side \n
   * (i.e. white maximizing the score, and black minimizing it)
   */
  [[nodiscard]] inline int evaluate() const { return evaluateBoard(board, current_move_maker); }

  /**
   * @brief Checking who can currently move their pieces
   *
   * @return true if the player can currently make a move, false if the bot can make the next move
   */
  [[nodiscard]] constexpr bool isPlayerTurn() { return bot_color != current_move_maker; }

  /**
   * @return the amount of moves that can be undone (i.e. how many moves have been played so far)
   */
  [[nodiscard]] inline uint64_t amountOfUndoableMoves() { return history.size(); }
};
