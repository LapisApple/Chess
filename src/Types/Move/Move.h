//
// Created by timap on 25.01.2024.
//

#pragma once

#include <cstdint>

#include "../BasicChessTypes.h"
#include "../SlimOptional.h"

/**
 * @brief Represents a move in a chess game
 *
 * @note A Move consists of the piece being moved, the initial and final positions of the piece on the board,
 * and an optional field for pawn promotion and another for special moves like castling or en passant.
 */
struct Move {
  /**
   * The Piece that is being moved
   */
  Piece piece;
  /**
   * the square where the moving Piece is located before the move (0=..\<64)
   */
  int8_t from;
  /**
   * the square where the moving Piece is located after the move (0=..\<64)
   */
  int8_t to;
  /**
   * An optional field that, if present, indicates the PieceType to which a pawn is being promoted
   */
  SlimOptional<PieceType::PieceType> promote;
  /**
   * An optional field that, if present, indicates a special move (e.g., castling, en passant)
   */
  SlimOptional<SpecialMove> specialMove;
};

/**
 * @brief A simple wrapper to get an empty Optional
 * @return A SlimOptional\<Move\> object representing an illegal / impossible Move
 */
template <>
constexpr inline SlimOptional<Move> SlimOptional<Move>::nullopt() noexcept {
  return SlimOptional({Piece::getEmpty(), -1, -1, OptionalPieceType::nullopt(), SlimOptional<SpecialMove>::nullopt()});
}

/**
 * @brief Checks if the Move is not a nullOpt
 * @return true if the Move is not impossible and false otherwise
 */
template <>
constexpr inline bool SlimOptional<Move>::has_value() const noexcept {
  const bool result_condition = data.to >= 0 && data.to < 64;
#ifndef NO_ASSERTS
  const bool extensive_condition = (data.to >= 0 && data.to < 64 && data.from >= 0 && data.from < 64 &&
                                    data.piece.team != Team::NONE && data.piece.type != PieceType::NONE);
  assert(result_condition == extensive_condition);
#endif
  return result_condition;
}
