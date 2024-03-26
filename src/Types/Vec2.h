//
// Created by LapisApple on 1/12/24.
//

#pragma once
#include <cassert>
#include <cstdint>

/** @brief a simple struct to represent a 2D position on the board
 *
 * the position starts on the top left of the board. (x goes to the right up to 7, y down and up to 7)
 */
struct Vec2 {
  int8_t x, y;

  /**
   * @param x,y numbers preferably representing a row and column of the board (0..=7) or a direction
   * @return a new Vec2 with the arguments as the values for x and y
   */
  constexpr inline Vec2(int8_t x, int8_t y) noexcept : x(x), y(y) {}

  /**
   * @return the position of the Vec2 on the Board
   */
  [[nodiscard]] constexpr inline int8_t getPos() const noexcept {
#ifndef NO_ASSERTS
    assert(!this->outsideBoard());
#endif
    return static_cast<int8_t>(y * 8 + x);
  }

  /**
   * @return true if the position described by the Vec2 is inside the chess board, otherwise false
   */
  [[nodiscard]] constexpr inline bool outsideBoard() const noexcept {
    constexpr int8_t bitsOutsideBoard = ~7;
    return (x | y) & bitsOutsideBoard;
  }

  /**
   * adds two Vec2 together and returns the result
   */
  constexpr inline Vec2 operator+(Vec2 rhs) const noexcept {
    return {static_cast<int8_t>(this->x + rhs.x), static_cast<int8_t>(this->y + rhs.y)};
  }

  /**
   * adds two Vec2 together by calling the + operator
   */
  constexpr inline void operator+=(Vec2 rhs) noexcept { *this = *this + rhs; }

  /**
   * @param from : the starting field on the board (value >= 0 and less than 64)
   * @param to : the end field on the board (value >= 0 and less than 64)
   * @return A 2D Vector representing the direction from "from" to "to"
   */
  constexpr static inline Vec2 newVec2(int8_t from, int8_t to) noexcept {
    int x = (to & 7) - (from & 7);
    int y = (to >> 3) - (from >> 3);

    Vec2 direction = Vec2(static_cast<int8_t>(x), static_cast<int8_t>(y));

    return direction;
  }

  /**
   * @param pos : a position on the board (value >= 0 and less than 64)
   * @return A 2D Vector representing the position of pos
   */
  constexpr static inline Vec2 newVec2(int8_t pos) noexcept {
    int x = (pos & 7);
    int y = (pos >> 3);

    Vec2 direction = Vec2(static_cast<int8_t>(x), static_cast<int8_t>(y));

    return direction;
  }
};
