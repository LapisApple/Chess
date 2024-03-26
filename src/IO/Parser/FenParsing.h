//
// Created by timap on 27.01.2024.
//

#pragma once
#include <climits>
#include <string_view>

#include "../../Types/BasicChessTypes.h"
#include "../../Types/SlimOptional.h"
#include "FenParsingException.h"

/**
 * @brief Class for reading a FEN string character by character.
 * \n
 * This class provides methods for iterating over a FEN string, skipping whitespace, and accessing the current
 * character. It also provides methods for checking if there are any characters left in the string and if the current
 * character is a whitespace.
 */
class FenStringReader {
 private:
  /**
   * @brief The underlying FEN as a string_view
   */
  std::string_view str;
  /**
   * @brief The of the current char
   */
  int offset;
  /**
   * @brief The size of the underlying FEN
   */
  const int size;

 public:
  // constructor
  /**
   * @brief Constructor for FenStringReader.
   *
   * @param str The FEN string to read
   */
  constexpr inline explicit FenStringReader(std::string_view str) noexcept
      : str(str), offset(0), size(static_cast<int>(str.size())) {
#ifndef NO_ASSERTS
    // the size of the given string_view should under pretty much all circumstances not exceed the signed integer limit
    // this assert is here just to be sure, that this assumption is correct
    assert(str.size() <= INT32_MAX);
#endif
  }

  // iteration operators
  /**
   * @brief Overloaded prefix increment operator.
   *
   * This operator increments the offset of the FenStringReader by one,
   * effectively moving the reader to the next character in the FEN string.
   *
   * @return A reference to the FenStringReader object
   */
  constexpr inline FenStringReader& operator++() noexcept {
    offset++;
    return *this;
  };

  /**
   * @brief Overloaded addition assignment operator.
   *
   * This operator increments the offset of the FenStringReader by a specified number,
   * effectively moving the reader i characters forward.
   *
   * @param i The number of characters to move forward
   */
  constexpr inline void operator+=(int i) noexcept { offset += i; };

  // helper methods
  /**
   * @brief Checks if there are any characters left in the FEN string.
   *
   * @return true if there are characters left in the FEN string, false otherwise
   */
  [[nodiscard]] constexpr inline bool hasCharsLeft() const noexcept { return offset < size && offset >= 0; }

  /**
   * @brief Returns the number of characters left in the FEN string.
   *
   * @return The number of characters left in the FEN string
   */
  [[nodiscard]] constexpr inline int amountCharsLeft() const noexcept { return size - offset; }

  /**
   * @brief Checks if the current character in the FEN string is a whitespace.
   *
   * @return true if the current character is a whitespace, false otherwise
   */
  [[nodiscard]] constexpr inline bool currentCharIsWhiteSpace() const noexcept {
    return this->get() == ' ' || this->get() == '\t';
  }

  /**
   * @brief skips forward in the FEN until a non whitespace character is found.
   * (i.e. skipping all current whitespaces)
   */
  constexpr void skipWhiteSpace() {
    for (; this->hasCharsLeft(); ++(*this)) {
      if (!currentCharIsWhiteSpace()) break;
    }
  }
  /**
   * @brief skips forward in the FEN until a non whitespace character is found (i.e. skipping all current whitespaces).
   * And throws a MissingDataInFen() if teh FEN ends before a non whitespace character is found
   */
  constexpr void skipWhiteSpaceAndExpectFurtherData() {
    this->skipWhiteSpace();
    if (!this->hasCharsLeft()) throw MissingDataInFen();
  }

  // access
  /**
   * @brief Access to the current Offset (i.e. the index of the current char).
   *
   * @return The current offset in the FEN string
   */
  [[nodiscard]] constexpr inline int getCurrentOffset() const noexcept { return offset; }

  /**
   * @brief Returns the current character in the FEN string without changing the offset,
   * meaning that calling this method multiple times will always return the same char
   *
   * @return The current character in the FEN string
   */
  [[nodiscard]] constexpr inline char get() const {
    if (!this->hasCharsLeft()) throw InternalOutOfBoundsErrorFen();
    return str[offset];
  }

  /**
   * @brief direct access to the underlying FEN with the index being offset by the private offset variable
   * @param i The index of the char to return relativ to the current char that can be get with get()
   * @return The character at the specified position in the FEN string relativ to the current offset
   */
  constexpr inline char operator[](int i) const {
    const int pos = offset + i;
    if (pos >= size || pos < 0) throw InternalOutOfBoundsErrorFen();
    return str[pos];
  }
};

/**
 * @brief Struct representing a Chess Board in FEN Notation.
 */
struct Fen {
  /**
   * @brief The Chess Board part in FEN notation.
   * @note should not be changed manually!
   */
  std::string_view board = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
  /**
   * @brief The player, who can currently make a move.
   * @note should not be changed manually!
   */
  Team::Team current_player = Team::WHITE;
  /**
   * @brief the current castling rights, represented as bit flags.
   * @note should not be changed manually!
   */
  uint8_t castling = 0b1111;
  /**
   * @brief A possible en passant Position.
   * @note should not be changed manually!
   */
  ChessPos en_passant = ChessPos::nullopt();
  /**
   * @brief number of half moves since the last pawn move or capture.
   * @note for the 50 move rule (that is currently not implemented)
   * @note should not be changed manually!
   */
  int amount_half_moves = 0;
  /**
   * @brief The number of the full moves in the game.
   * @note starts at 1 and gets incremented after each Black's move
   * @note should not be changed manually!
   */
  int current_turn = 1;

  /**
   * @brief Default constructor for Fen. \n
   * constructs the starting Chess Game State
   */
  constexpr Fen() = default;

  /**
   * @brief Static method for building the corresponding Fen object from a string in FEN notation.
   *
   * @param str The FEN string to parse
   * @return A Fen object representing the string in FEN notation
   */
  static Fen buildFenFromStr(std::string_view str);
};
