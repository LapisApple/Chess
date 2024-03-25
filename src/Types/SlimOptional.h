//
// Created by timap on 25.01.2024.
//

#pragma once

#include <cstdint>

#include "../Defines.h"
#include "BasicChessTypes.h"

/**
 * @brief a simple zero overhead Optional for Predefined Types
 *
 * works by simpy wrapping the data in a class and making nullopt be illegal states of the base Type T
 *
 */
template <class T>
class SlimOptional {
 public:
  /**
   * @brief the data held by the class
   */
  T data;

  /**
   * @brief a simple constructor that just sets this->data to the argument
   */
  constexpr explicit SlimOptional(T data) : data(data) {}

  /**
   * @brief A simple wrapper to get an empty Optional
   * @return returns a Null Optional of the specified type
   */
  static constexpr inline SlimOptional<T> nullopt() noexcept = delete;

  /**
   * @return true if the data is not in an illegal or missing state.
   * @example T = int8_t: true if a Position is actually inside the Chess board
   */
  [[nodiscard]] constexpr inline bool has_value() const noexcept = delete;
};

// Position
/**
 * @brief A type alias for SlimOptional<int8_t> in order to better represent its uses
 */
using ChessPos = SlimOptional<int8_t>;

/**
 * @brief A simple wrapper to get an empty Optional
 * @return A SlimOptional\<int8_t\> object representing an illegal chess position (i.e., a position outside the chess
 * board)
 */
template <>
constexpr inline ChessPos ChessPos::nullopt() noexcept {
  return SlimOptional<int8_t>(-1);
}

/**
 * @brief Checks if the chess position is valid
 * @return true if position is inside the Chess Board and false otherwise
 */
template <>
constexpr inline bool ChessPos::has_value() const noexcept {
  return data >= 0 && data < 64;
}

// Natural Number
/**
 * @brief A type alias for SlimOptional<int> in order to better represent its uses
 */
using NaturalNumber = SlimOptional<int>;

/**
 * @brief A simple wrapper to get an empty Optional
 * @return A SlimOptional\<int\> object representing an illegal Natural Number (i.e., a negative number)
 */
template <>
constexpr inline NaturalNumber NaturalNumber::nullopt() noexcept {
  return SlimOptional<int>(-1);
}

/**
 * @brief Checks if the underlying number is natural
 * @return true if the number is natural (i.e. \>=0) and false otherwise
 */
template <>
constexpr inline bool NaturalNumber::has_value() const noexcept {
  return data >= 0;
}

// Piece
/**
 * @brief A simple wrapper to get an empty Optional
 * @return A SlimOptional\<Piece\> object representing a missing Piece (i.e., a piece with Team::NONE)
 */
template <>
constexpr inline SlimOptional<Piece> SlimOptional<Piece>::nullopt() noexcept {
  return SlimOptional<Piece>(Piece::getEmpty());
}

/**
 * @brief Checks if the underlying Piece is an actual Chess Piece and not None
 * @return true if the Piece is part of either team white or team black and false otherwise
 */
template <>
constexpr inline bool SlimOptional<Piece>::has_value() const noexcept {
  const bool result_condition = data.team != Team::NONE;
#ifndef NO_ASSERTS
  const bool extensive_condition = data.team != Team::NONE && data.type != PieceType::NONE;
  assert(result_condition == extensive_condition);
#endif
  return result_condition;
}

// PieceType
/**
 * @brief A type alias for SlimOptional<PieceType::PieceType>
 * due to the original typename just being far too long
 */
using OptionalPieceType = SlimOptional<PieceType::PieceType>;

/**
 * @brief A simple wrapper to get an empty Optional
 * @return A SlimOptional\<Piece\> object representing a missing Piece (i.e., a piece with Team::NONE)
 */
template <>
constexpr inline OptionalPieceType OptionalPieceType::nullopt() noexcept {
  return SlimOptional(PieceType::NONE);
}

/**
 * @brief Checks if the underlying PieceType is an actual Chess Piece type and not None
 * @return true if the PieceType is not None and false otherwise
 */
template <>
constexpr inline bool OptionalPieceType::has_value() const noexcept {
  return data != PieceType::NONE;
}

// SpecialMove
/**
 * @brief A simple wrapper to get an empty Optional
 * @return A SlimOptional\<Piece\> object representing a missing SpecialMove (i.e., SpecialMove::NONE)
 */
template <>
constexpr inline SlimOptional<SpecialMove> SlimOptional<SpecialMove>::nullopt() noexcept {
  return SlimOptional(SpecialMove::NONE);
}

/**
 * @brief Checks if the underlying SpecialMove is an actual SpecialMove and not None
 * @return true if the SpecialMove is not None and false otherwise
 */
template <>
constexpr inline bool SlimOptional<SpecialMove>::has_value() const noexcept {
  return data != SpecialMove::NONE;
}
