//
// Created by timap on 22.01.2024.
//

#pragma once
#include <cassert>
#include <string_view>

#include "../../Types/BasicChessTypes.h"
#include "../../Types/SlimOptional.h"
#include "../../Types/Move/Move.h"
#include "../../IO/Parser/StringParsing.h"
#include "../../Types/Move/ExecutedMove.h"

/*
*     array indexes:
*         (A)(B)(C)(D)(E)(F)(G)(H)
*     (8) [0][1][2][3][4][5][6][7]
*     (7) [8][9][ ][ ][ ][ ][ ][ ]
*     (6) [16][ ][ ][ ][ ][ ][ ][ ]
*     (5) [24][ ][ ][ ][ ][ ][ ][ ]
*     (4) [32][ ][ ][ ][ ][ ][ ][ ]
*     (3) [40][ ][ ][ ][ ][ ][ ][ ]
*     (2) [48][ ][ ][ ][ ][ ][ ][ ]
*     (1) [56][ ][ ][ ][ ][ ][ ][63]
*/
/**
 * @brief a simple data structure to represent the board in Chess as an array with 64 fields of Pieces
 * @note array indexes start at the top left and go left for 8 fields,
 * afterwards starting a new line on the left right below the last line
 * the last field is on the bottom right with an index of 63
 */
class Board_8x8 {
private:
    /**
     * @brief the array with 64 fields of Pieces that represents the Chess Board of size 8x8
     */
    Piece board[64];
public:

//constructors
    /**
     * @brief constructs the Board_grid of the starting state in Chess
     */
    explicit constexpr Board_8x8() noexcept: Board_8x8("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR") {
    }

    /**
     * @brief constructs a Board_grid with the given parameters
     * @param fen_board the board part of the FEN as given by Fen.board, meaning a legal board without any illegal characters and whitespace
     * @note giving an illegal board state produces undefined behaviour due to this function not checking its parameters
     */
    explicit constexpr Board_8x8(const std::string_view& fen_board) noexcept {
        int offset = 0;
        for (int i = 0; i < fen_board.size(); ++i) {
            char ch = fen_board[i];
            // don't go forward a field if '/'
            if (ch == '/') {
                offset--;
                continue;
            }
            // try parsing figure
            SlimOptional<Piece> piece = pieceFromChar(ch);
            if (piece.has_value()) {
                board[i + offset] = piece.data;
                continue;
            }
            // otherwise ch == number -> go forward a few fields
            // note: empty fields don't need to be explicitly filled due to
            // the default constructor producing empty fields
            offset += ch - '1';

        }
    }
// setters
    // direct access operator
    /**
     * @brief direct access to the underlying array of Pieces
     * @param i the index of the piece to be accessed
     * @note bounds checks can be disabled by defining NO_BOUNDS_CHECKS
     */
    constexpr inline Piece operator[](int i) const noexcept {
#ifndef NO_BOUNDS_CHECKS
        assert(i >= 0 && i < 64);
#endif
        return board[i];
    };

    /**
     * @brief executes the given move and updates any needed values
     * @param move the move to be executed
     * @param pos_of_passantable_pawn a custom Optional of the actual position of the piece that would be taken by en passant
     * @note does not check if the move and/or pos_of_passantable_pawn piece are actually valid
     * @return: the captured Piece or if no Piece was captured the Piece overwritten by the move
     */
    constexpr Piece movePiece(Move move, ChessPos pos_of_passantable_pawn) noexcept {
        Piece capture = board[move.to];

        board[move.to] = board[move.from];
        board[move.from] = Piece::getEmpty();

        if (move.promote.has_value()) board[move.to].type = move.promote.data;

        switch (move.specialMove.data) {
            case SpecialMove::en_Passant:
                // execute en passant capture
#ifndef NO_ASSERTS
                assert(pos_of_passantable_pawn.has_value());
#endif
                capture = board[pos_of_passantable_pawn.data];
                board[pos_of_passantable_pawn.data] = Piece::getEmpty();
                break;
            case SpecialMove::CastleKingSide:
                // move king side castle to protect the king
                board[move.from + 1] = board[move.from + 3];
                board[move.from + 3] = Piece::getEmpty();
                break;
            case SpecialMove::CastleQueenSide:
                // move queen side castle to protect the king
                board[move.from - 1] = board[move.from - 4];
                board[move.from - 4] = Piece::getEmpty();
                break;
            default:
                break;
        }

        return capture;
    }

    /**
     * @brief undoes a previously executed Move
     * @param move the executed move to be undone
     * @note does not check if the move is actually valid and the previous move
     */
    constexpr void undoMove(ExecutedMove move) noexcept {

        // unmove piece
        board[move.move.to] = Piece::getEmpty();
        // that already takes care of undoing promotion
        board[move.move.from] = move.move.piece;

        //un-capture
        if (move.capture.has_value()) {
            const bool was_move_en_passant = move.move.specialMove.data == SpecialMove::en_Passant;
#ifndef NO_ASSERTS
            if (was_move_en_passant) assert(move.extra.getPosOfPassantablePiece().has_value());
#endif
            const int8_t capture_pos = was_move_en_passant ? move.extra.getPosOfPassantablePiece().data : move.move.to;
            board[capture_pos] = move.capture.data;
        }

        // un-castle king
        switch (move.move.specialMove.data) {
            case SpecialMove::CastleKingSide:
                board[move.move.from + 3] = board[move.move.from + 1];
                board[move.move.from + 1] = Piece::getEmpty();
                break;
            case SpecialMove::CastleQueenSide:
                board[move.move.from - 4] = board[move.move.from - 1];
                board[move.move.from - 1] = Piece::getEmpty();
                break;
            default:
                break;
        }
    }
};

