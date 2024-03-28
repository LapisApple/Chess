//
// Created by timap on 27.01.2024.
//

#pragma once

#include <cassert>
#include <vector>

#include "../Board/Board.h"
#include "../Check/Check.h"
#include "../Types/BasicChessTypes.h"
#include "../Types/Move/Move.h"
#include "../Types/Vec2.h"

namespace PossibleMoves {
/**
 * @brief Generates all Pseudo-legal moves for a given team on a given board
 *
 * @param board The current state of the chess board
 * @param moveVec A vector to add the generated moves to
 * @param team The team for which to generate the moves
 *
 * @return nothing, but modifies the given vector of moves
 */
void getAllPossibleMoves(const Board& board, std::vector<Move>& moveVec, Team::Team team);

#ifndef STANDARD_CHESS

constexpr int surroundingKingAmount(const Board& board, Vec2 pos) {
    int kingAmount = 0;
    for (int8_t x = -1; x <= 1; ++x) {
        for (int8_t y = -1; y <= 1; ++y) {
            Vec2 offset = Vec2(x, y);
            Vec2 capturePos = pos + offset;

            if (capturePos.outsideBoard()) continue;
            if (board.grid[capturePos.getPos()].type == PieceType::KING) kingAmount++;
        }
    }
    return kingAmount;
}

constexpr void trimBothKingsErasedAtOnce(const Board& board, std::vector<Move>& move_list) {
#ifndef NO_ASSERTS
    assert(move_list.size() <= INT64_MAX);
#endif
    int64_t move_list_size = static_cast<int64_t>(move_list.size());
    for (int64_t i = move_list_size - 1; i >= 0; --i) {
        const Move& move = move_list[i];

        if (!board.isMoveCapture(move)) continue;

        if (surroundingKingAmount(board, Vec2::newVec2(move.to)) > 1) move_list.erase(move_list.begin() + i);
    }
}

    constexpr void trimKingCapture(const Board& board, std::vector<Move>& move_list) {
#ifndef NO_ASSERTS
    assert(move_list.size() <= INT64_MAX);
#endif
    int64_t move_list_size = static_cast<int64_t>(move_list.size());
    for (int64_t i = move_list_size - 1; i >= 0; --i) {
        const Move& move = move_list[i];

        if ((move.piece.type == PieceType::KING) && (board.grid[move.to].team != Team::NONE)) {
            move_list.erase(move_list.begin() + i);
        }
    }
}

inline void getAllPossibleAtomicMoves(const Board& board, std::vector<Move>& moveVec, Team::Team team) {
    getAllPossibleMoves(board, moveVec, team);
    trimKingCapture(board,moveVec);
    trimBothKingsErasedAtOnce(board, moveVec);
}
#endif

/**
 * @brief removes all Moves that would put the player in checkmate
 *
 * @param board the current chess board state
 * @param move_list a list of move from which moves checkmating the player should be removed
 * @param player the player for which to check
 *
 * @return nothing, but removes moves putting the player in checkmate
 */
inline void trimMovesPuttingPlayerIntoCheckmate(const Board& board, std::vector<Move>& move_list, Team::Team player) {
#ifndef NO_ASSERTS
  assert(move_list.size() <= INT64_MAX);
#endif
  int64_t move_list_size = static_cast<int64_t>(move_list.size());
  for (int64_t i = move_list_size - 1; i >= 0; --i) {
    Board tempBoard = board;
    const Move current_move = move_list[i];
    tempBoard.movePiece(current_move);

    const bool is_checked = Check::isChecked(tempBoard, player, tempBoard.positions.getKingPos(player));

    if (is_checked) move_list.erase(move_list.begin() + i);
  }
}
}  // namespace PossibleMoves
