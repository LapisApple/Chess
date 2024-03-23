//
// Created by timap on 27.01.2024.
//

#pragma once

#include <cassert>
#include <vector>
#include "../Types/BasicChessTypes.h"
#include "../Types/Move/Move.h"
#include "../Board/Board.h"
#include "../Types/Vec2.h"
#include "../Check/Check.h"

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
}

