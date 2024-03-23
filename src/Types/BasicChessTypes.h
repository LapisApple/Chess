//
// Created by LapisApple on 1/16/24.
//

#pragma once

#include <cassert>
#include <cstdint>
#include "../Defines.h"

/**
* @brief the amount of different Piece types that exist (i.e. including None)
*/
#define PIECETYPE_AMOUNT 7
/**
* @brief the amount of different teams that exist  (i.e. excluding None)
*/
#define TEAM_AMOUNT 2
/**
* @brief the amount of pieces each team has
*/
#define PIECES_PER_TEAM 16
/**
* @brief the amount of squares on a Chess Board
*/
#define BOARD_SIZE 64

/**
* @brief the namespace holding the PieceType enum in order to not pollute the main namespace
*/
namespace PieceType {
/**
* @brief an enum representing the possible types of a Piece, and a possibly missing type
*/
enum PieceType: uint8_t {
  NONE = 0,
  KING,
  QUEEN,
  KNIGHT,
  BISHOP,
  CASTLE,
  PAWN = 6
};
}

/**
* @brief the namespace holding the team enum in order to not pollute the main namespace
*/
namespace Team  {
/**
* @brief an enum representing team white, team black, and a possibly missing team
*/
enum Team : uint8_t {
  BLACK = 0,
  WHITE = 1,
  NONE,
};

/**
 *  @param team : a Team. Either Team::WHITE or TEAM::BLACK.
 *  @note undefined behaviour for inputs other than Team::WHITE or TEAM::BLACK
 *  @return   the enemy team of the team in the argument.
 *
 *  In: WHITE -> Out: BLACK \n
 *  In: BLACK -> Out: WHITE
 */
constexpr inline Team getEnemyTeam(Team team) noexcept {
#ifndef NO_ASSERTS
    assert(team == Team::WHITE || team == Team::BLACK);
#endif
    return static_cast<Team>(!team);
}

}

/**
* @brief an enum representing all possible special moves
*/
enum class SpecialMove : uint8_t {
  NONE = 0b0,
  loseCastlingKingSide = 0b01,
  loseCastlingQueenSide = 0b10,
  loseCastlingBoth = 0b11,
  en_Passant,
  pawnMove2,
  CastleKingSide,
  CastleQueenSide,
};

/**
* @brief a simple way to represent a Chess Piece
 * with both a team variable and a type variable
*/
struct Piece {
    /**
     * @brief the team of the piece
     */
    Team::Team team : 3;

    /**
     * @brief the type of the piece
     */
    PieceType::PieceType type : 5;

    /**
     * @brief a simple default constructor for Piece
     *  @return a constructed Piece with Team::NONE and PieceType::NONE
     */
    constexpr explicit Piece() noexcept : team(Team::NONE), type(PieceType::NONE) {}

    /**
     * @brief a simple constructor for Piece
     * @param team the team of the Piece to be constructed
     * @param type the pieceType of the Piece to be constructed
     *  @return a constructed Piece with the given parameters
     */
    constexpr explicit Piece(Team::Team team, PieceType::PieceType type) noexcept : team(team), type(type) {}

    /**
     * @brief a simple helper method to make certain that an empty Piece is returned
     *  @return an empty Piece with both Team::NONE and PieceType::NONE
     */
    static constexpr Piece getEmpty() noexcept {
        return Piece(Team::NONE, PieceType::NONE);
    }

/**
 *  @param other : a piece different from the current one
 *  @return   true if and only if both pieces have the same team and type, otherwise false
 */
    constexpr bool operator==(Piece other) const {
        return this->team == other.team && this->type == other.type;
    }
/**
 *  @param other : another piece
 *  @return   false if and only if both pieces have the same team and type, otherwise true
 */
    constexpr bool operator!=(Piece other) const {
        return this->team != other.team || this->type != other.type;
    }

 /**
 *  @return  the given Piece as it would be represented by a character
 */
 [[nodiscard]] constexpr char asChar() const {
     constexpr char piece_to_char_map[3][PIECETYPE_AMOUNT] = {
             {' ','k', 'q', 'n', 'b', 'r', 'p'},
             {' ','K', 'Q', 'N', 'B', 'R', 'P'},
             {' ',' ', ' ', ' ', ' ', ' ', ' '},
     };
     return piece_to_char_map[this->team][this->type];
 }
};

