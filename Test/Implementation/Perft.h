//
// Created by LapisApple on 3/17/24.
//

#pragma once
#include "../../src/Board/Board.h"
#include "../../src/PossibleMoves/PossibleMoves.h"
#include "../../src/Check/Check.h"
#include "../../src/IO/Print/Print.h"
#include "../Main_Folder_Path_For_Testing.h"
#include <vector>
#include <cstdint>

// #define DEBUG_PERFT
// #define CREATING_MOVE_TEST_DATA
namespace Impl {
    uint64_t Perft_impl(Board board, int depth, Team::Team team, Move prev_move) {
        Team::Team enemy = Team::getEnemyTeam(team);
        // get the new board
#ifdef CREATING_MOVE_TEST_DATA
        const bool is_capture = board.grid[prev_move.to].team != Team::NONE || prev_move.specialMove.data == SpecialMove::en_Passant;
        std::string fen_prev = Print::board_state_to_fen(board.grid, board.extra, prev_move.piece.team, 1, 0) + ";";
#endif
        board.movePiece(prev_move);
#ifdef DEBUG_PERFT
        board.print_fen(std::cout, team, 1, 0);
#endif
        if (Check::isChecked(board, enemy, board.positions.getKingPos(enemy)))
            return 0;

#ifdef CREATING_MOVE_TEST_DATA
        std::ofstream file(main_folder_path + "/Test/gtest/datasets/move_data.txt", std::ios::out | std::ios::app);
        file << fen_prev;
        file << Print::move_as_string(prev_move, is_capture) << ";";
        file << Print::board_state_to_fen(board.grid, board.extra, Team::getEnemyTeam(prev_move.piece.team), 1, 0) << '\n';
#endif

        if (depth <= 0) return 1;
        uint64_t amount_boards = 0;


        std::vector<Move> move_list;
        PossibleMoves::getAllPossibleMoves(board, move_list, team);

        for (const Move next_move: move_list) {
            amount_boards += Perft_impl(board, depth - 1, enemy, next_move);
        }
        return amount_boards;
    }
}

uint64_t Perft(Board board, int depth, Team::Team team) {
  Team::Team enemy = Team::getEnemyTeam(team);
  if (!board.positions.hasPiece(Team::WHITE, PieceType::KING) || !board.positions.hasPiece(Team::BLACK, PieceType::KING)) return 0;

  if (depth <= 0) return 1;
  uint64_t amount_boards = 0;

  std::vector<Move> move_list;
  PossibleMoves::getAllPossibleMoves(board, move_list, team);

  for (const Move move : move_list) {
    amount_boards += Impl::Perft_impl(board, depth - 1, enemy, move);
  }
  return amount_boards;
}

namespace Impl {
    uint64_t Perft_withUndo_impl(Board& board, int depth, Team::Team team) {
        if (depth <= 0) return 1;

        const Team::Team enemy = Team::getEnemyTeam(team);
        uint64_t amount_boards = 0;

        std::vector<Move> move_list;
        PossibleMoves::getAllPossibleMoves(board, move_list, team);

        for (const Move move : move_list) {
            const ExecutedMove done_move = board.movePiece(move);
            if (!Check::isChecked(board, team, board.positions.getKingPos(team)))
                amount_boards += Impl::Perft_withUndo_impl(board, depth - 1, enemy);
            board.undoMove(done_move);
        }
        return amount_boards;
    }
}


uint64_t Perft_withUndo(Board& board, int depth, Team::Team team) {
    const Team::Team enemy = Team::getEnemyTeam(team);

    // is it even a legal game state?
    if (!board.positions.hasPiece(Team::WHITE, PieceType::KING) || !board.positions.hasPiece(Team::BLACK, PieceType::KING)) return 0;
    // has the game already ended?
    if (Check::isChecked(board, enemy, board.positions.getKingPos(enemy))) return 0;
    // after invariance is established call the actual Perft implementation
    const uint64_t perft_res = Impl::Perft_withUndo_impl(board, depth, team);
    return perft_res;
}

uint64_t Perft_debug(Board board, int depth, Team::Team team) {
    Team::Team enemy = Team::getEnemyTeam(team);
    if (!board.positions.hasPiece(Team::WHITE, PieceType::KING) || !board.positions.hasPiece(Team::BLACK, PieceType::KING)) return 0;

    if (depth <= 0) return 1;
    uint64_t amount_boards = 0;

    std::vector<Move> move_list;
    PossibleMoves::getAllPossibleMoves(board, move_list, team);

    for (const Move move : move_list) {
#ifdef DEBUG_PERFT
        std::cout << move_to_char(move.from) << move_to_char(move.to) << ' ' << std::endl;
#endif
        uint64_t amount = Impl::Perft_impl(board, depth - 1, enemy, move);
#ifdef DEBUG_PERFT
        std::cout << amount << std::endl;
#else
        std::cout << Print::position_to_string(move.from) << Print::position_to_string(move.to) << ' ' << amount << std::endl;
#endif
        amount_boards += amount;
    }
    return amount_boards;
}