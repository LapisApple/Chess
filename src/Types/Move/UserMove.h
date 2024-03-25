//
// Created by timap on 22.03.2024.
//

#pragma once

#include <cstdint>

#include "../BasicChessTypes.h"
#include "../SlimOptional.h"
#include "Move.h"

/**
 * @brief Represents a move made by a player in a chess game
 *
 * @note A Move consists of the piece being moved, the initial and final positions of the piece on the board,
 * whether the move captures a piece, and an optional field for pawn promotion.
 */
struct UserMove {
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
   * a bool that indicates whether this move results in a capture
   */
  bool is_capture;
  /**
   * An optional field that, if present, indicates the PieceType to which a pawn is being promoted
   */
  SlimOptional<PieceType::PieceType> promote;

  /**
   * A default constructor that creates an impossible/illegal player move
   */
  constexpr UserMove() noexcept
      : piece(Piece::getEmpty()), from(-1), to(-1), is_capture(false), promote(OptionalPieceType::nullopt()) {}

  /**
   * @brief a helper method to compare a UserMove with a normal Move
   * @param other the move to be compared to
   * @param other_is_capture whether the normal move results in a capture,
   * because move does not hold such data
   * @return true if the User move is the same as the normal move when disregarding specialMove data, and false
   * otherwise
   */
  [[nodiscard]] constexpr inline bool equals(Move other, bool other_is_capture) const {
    return this->piece == other.piece && this->from == other.from && this->to == other.to &&
           this->promote.data == other.promote.data && this->is_capture == other_is_capture;
  }
};

/**
 * @brief A simple wrapper to get an empty Optional
 * @return A SlimOptional\<UserMove\> object representing an illegal/impossible Player Move
 */
template <>
constexpr inline SlimOptional<UserMove> SlimOptional<UserMove>::nullopt() noexcept {
  return SlimOptional(UserMove());
}
/**
 * @brief Checks whether the Move is an empty Optional
 * @return true if the Player Move is not impossible and false otherwise
 */
template <>
constexpr inline bool SlimOptional<UserMove>::has_value() const noexcept {
  const bool result_condition = data.to >= 0 && data.to < 64;
#ifndef NO_ASSERTS
  const bool extensive_condition = (data.to >= 0 && data.to < 64 && data.from >= 0 && data.from < 64 &&
                                    data.piece.team != Team::NONE && data.piece.type != PieceType::NONE);
  assert(result_condition == extensive_condition);
#endif
  return result_condition;
}
