//
// Created by timap on 27.01.2024.
//

#pragma once

#include <cstdint>

#include "../Board/Board.h"
#include "../Types/BasicChessTypes.h"

namespace Check {
/**
 * @brief Checks if a player is in check
 *
 * @param board The current state of the chess board
 * @param player The player to check for being in Check (Team::WHITE or Team::BLACK)
 * @param the position of the players king
 * @return true if the player is in Check, otherwise false
 */
bool isChecked(const Board& board, Team::Team team, int8_t king_pos) noexcept;

#ifndef  STANDARD_CHESS

constexpr bool hasLostKing(const Board& board) noexcept {
    if (!board.positions.hasPiece(Team::WHITE, PieceType::KING)) return true;
    if (!board.positions.hasPiece(Team::BLACK, PieceType::KING)) return true;
    return false;
}

    constexpr int surroundingKingAmount(const Board& board, Vec2 pos) {
        int kingAmount = 0;

        auto calculateKingAmount = [&kingAmount, &board](int8_t pos) {
            if (board.grid[pos].type == PieceType::KING) kingAmount++;
        };

        forAllAdjacentPieces(pos, calculateKingAmount);
        return kingAmount;
    }


  inline  bool actualAtomicCheck(const Board& board, Team::Team player, int8_t king_pos) {
        Vec2 pos = Vec2::newVec2(king_pos);

        for (int8_t x = -1; x <= 1; ++x) {
            for (int8_t y = -1; y <= 1; ++y) {
                Vec2 offset = Vec2(x, y);
                Vec2 capturePos = pos + offset;

                if (capturePos.outsideBoard()) continue;

                int8_t newPos = capturePos.getPos();
                if (board.grid[newPos].team != player) continue;

                // 2x explode
                if (surroundingKingAmount(board, capturePos) > 1) continue;

                // normal check
                const bool check = Check::isChecked(board,player,newPos);
                if (check) return true;
            }
        }

        return false;
    }

#endif
}  // namespace Check
