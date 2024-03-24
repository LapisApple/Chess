//
// Created by timap on 13.03.2024.
//

#pragma once

#include <vector>
#include <iostream>
#include "Board/Board.h"
#include "IO/Parser/FenParsing.h"
#include "PossibleMoves/PossibleMoves.h"
#include "Bot/MinMax/MinMax.h"
#include "IO/Print/Print.h"
#include "Check/CheckMate.h"
#include "Types/Move/UserMove.h"
#include "Bot/Evaluation/Evaluate.h"

enum class Result: int8_t { SUCCESS, FAILURE };

struct History {
    ExecutedMove move = ExecutedMove();
    int half_turn_before_move = 0;
};

class ChessGame {
    Board board;
    Team::Team current_player = Team::WHITE;
    int current_turn;
    int number_of_half_moves_since_last_pawn_move_or_capture;
    std::vector<History> history;

    void executeMove(Move move) {
        // create history
        History current_history = History();
        current_history.half_turn_before_move = number_of_half_moves_since_last_pawn_move_or_capture;
        // make move on board
        current_history.move = board.movePiece(move);

        // update other values
        // turn
        if (current_player == Team::BLACK) current_turn++;
        // player to move
        current_player = Team::getEnemyTeam(current_player);
        // 50 move rule
        if (move.piece.type == PieceType::PAWN || board.isMoveCapture(move)) number_of_half_moves_since_last_pawn_move_or_capture = 0;
        else number_of_half_moves_since_last_pawn_move_or_capture++;

        // save previous State
        history.push_back(current_history);
    }
public:
// constructor
    explicit ChessGame(): board(Board()), current_turn(1), number_of_half_moves_since_last_pawn_move_or_capture(0) {}

    explicit ChessGame(const Fen& fen) {
        board = Board(fen);
        current_player = fen.current_player;
        current_turn = fen.current_turn;
        number_of_half_moves_since_last_pawn_move_or_capture = fen.amount_half_moves;
    }

// make moves
    Result make_move(UserMove move) {
        std::vector<Move> move_list;
        PossibleMoves::getAllPossibleMoves(board, move_list, current_player);
        PossibleMoves::trimMovesPuttingPlayerIntoCheckmate(board, move_list, current_player);

        for (Move& current_move : move_list) {
            if (move.equals(current_move, board.isMoveCapture(current_move))) {
                executeMove(current_move);
                return Result::SUCCESS;
            }
        }
        return Result::FAILURE;
    }

    Result let_bot_make_move() {
        SlimOptional<Move> move = getBestMove();
        if (!move.has_value()) return Result::FAILURE;
        executeMove(move.data);
        return Result::SUCCESS;
    }

// undo moves
    Result undo_move() {
        // check that there are moves that can be undone
        if (history.empty()) return Result::FAILURE;

        const History& last_elem = history.back();

        board.undoMove(last_elem.move);
        number_of_half_moves_since_last_pawn_move_or_capture = last_elem.half_turn_before_move;

        if (current_player == Team::WHITE) current_turn--;
        current_player = Team::getEnemyTeam(current_player);

        history.pop_back();
        return Result::SUCCESS;
    }

// print
    inline void print_current_board(std::ostream& os) const {
        Print::print_board_grid_state(board.grid, os);
        os << std::endl;
    }

    [[nodiscard]] inline std::string asFen() const {
        return Print::board_state_to_fen(board.grid, board.extra, current_player, current_turn, number_of_half_moves_since_last_pawn_move_or_capture);
    }


    void print_all_possible_moves(std::ostream& os, int amount_per_row) const {
        std::vector<Move> move_list;
        PossibleMoves::getAllPossibleMoves(board, move_list, current_player);
        PossibleMoves::trimMovesPuttingPlayerIntoCheckmate(board, move_list, current_player);

        const int highest_num_in_row = amount_per_row - 1;
        for (int i = 0; i < move_list.size(); ++i) {
            Move& move = move_list[i];
            os << Print::move_as_string(move, board.isMoveCapture(move));
            os << ((i % amount_per_row == highest_num_in_row) ? '\n' : ' ');
        }
        os << std::endl;
    }

    void printMove(std::ostream& os,const Move& move) {
        os << Print::move_as_string(move, board.isMoveCapture(move)) << std::endl;
    }

// helper Methods
    [[nodiscard]] inline SlimOptional<Move> getBestMove() const {
        const int amount_half_turns_since_start = current_turn * 2 + (current_player == Team::BLACK);
        return MinMax(board, 5, current_player, amount_half_turns_since_start);
    }

    [[nodiscard]] inline bool player_is_in_check() const {
        const bool is_checked = Check::isChecked(board, current_player, board.positions.getKingPos(current_player));
        return is_checked;
    }

    [[nodiscard]] inline bool player_is_in_checkmate() const {
        const bool is_checkmated = CheckMate::isCheckMated(board, current_player);
        return is_checkmated;
    }

    [[nodiscard]] inline bool enemy_is_in_checkmate() const {
        const bool is_checkmated = CheckMate::isCheckMated(board, Team::getEnemyTeam(current_player));
        return is_checkmated;
    }

    [[nodiscard]] constexpr int evaluate() const {
        return ::evaluate(board, current_player);
    }
};



