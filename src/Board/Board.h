//
// Created by timap on 27.02.2024.
//

#pragma once

#include "Board_impl/Board_Extra.h"
#include "Board_impl/Board_8x8.h"
#include "Board_impl/Board_Positions.h"
#include "../IO/Parser/FenParsingException.h"
#include "../IO/Parser/FenParsing.h"
#include "../IO/Print/Print.h"
#include "Board_Correctness.h"

/**
 * @brief the Chess board in its entirety
 * @note made up of extra data (en Passant, and Castling) and 2 different board representations: \n
 * 1. a grid of 64 fields with pieces on them \n
 * 2. and a data structure to get all positions of a piece of a specific team
*/

class Board {
public:
    /**
     * @brief the Chess Board as a grid of 64 squares
    */
    Board_8x8 grid;
    /**
     * @brief en Passant and castling data
    */
    Board_Extra extra;
    // note: positions needs to stay below grid for the constructor to work correctly
    /**
     * @brief a data structure to get all positions for a piece of a specific team
    */
    Board_Positions positions;
//constructors
    /**
     * @brief a simple constructor that constructs the starting Chess Board State
    */
    constexpr explicit Board() : grid(), extra(), positions() {
#ifndef NO_ASSERTS
        const bool equal = positions_equals_grid(grid, positions);
        assert(equal);
#endif
    }

    /**
     * @brief a constructor that takes a processed fen representation and creates the corresponding board
     * @param fen a valid FEN Chess Board created through the default constructor of Fen or the buildFenFromStr() method
    */
    constexpr explicit Board(const Fen& fen) noexcept : grid(Board_8x8(fen.board)), extra(fen.castling, fen.en_passant), positions(Board_Positions(grid)) {
#ifndef NO_ASSERTS
        const bool equal = positions_equals_grid(grid, positions);
        assert(equal);
#endif

    }
//getters
    /**
     * @brief check whether a move would currently capture something
     * @param move the move to be checked for capture
     * @return true if the move actually captures something otherwise false
     *
     */
    [[nodiscard]] constexpr inline bool isMoveCapture(Move move) const noexcept {
        return move.specialMove.data == SpecialMove::en_Passant || grid[move.to].team == Team::getEnemyTeam(move.piece.team);
    }
//setters
    /**
     * @brief executes the given move and updates any needed values
     * @param move the move to be executed
     * @note does not check if the move are actually valid
     * @return: the executed move for later undoing of the currently made move
     */
    constexpr ExecutedMove movePiece(Move move) {
        ExecutedMove executed_move = ExecutedMove();
        executed_move.move = move;
        executed_move.extra = this->extra;
#ifndef NO_ASSERTS
        const bool equal_pre = positions_equals_grid(grid, positions);
        assert(equal_pre);
#endif

        ChessPos pos_of_passantable_pawn = extra.getPosOfPassantablePiece();
        const Piece capture = grid.movePiece(move, pos_of_passantable_pawn);
        positions.movePiece(move, SlimOptional(capture), pos_of_passantable_pawn);
        extra.movePiece(move, capture);

#ifndef NO_ASSERTS
        const bool equal_post = positions_equals_grid(grid, positions);
        assert(equal_post);
#endif
        executed_move.capture = SlimOptional(    capture);
        return executed_move;
    }

    /**
     * @brief undoes given move and updates any needed values
     * @param move the executed move to be undone
     * @note does not check if the move are actually valid and whether the move was actually the last one executed
     */
    constexpr void undoMove(ExecutedMove move) {
#ifndef NO_ASSERTS
        const bool equal_pre = positions_equals_grid(grid, positions);
        assert(equal_pre);
#endif
        this->grid.undoMove(move);
        this->extra = move.extra;
        this->positions.undoMove(move);

#ifndef NO_ASSERTS
        const bool equal_post = positions_equals_grid(grid, positions);
        assert(equal_post);
#endif
    }
};

