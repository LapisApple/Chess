//
// Created by timap on 16.03.2024.
//

#pragma once
#include "../../Types/BasicChessTypes.h"
#include "../../Types/SlimOptional.h"
#include "../../Defines.h"
#include "../../Types/Move/UserMove.h"

/**
 * @brief Converts a character to a Piece object
 *
 * @param ch The character to convert to a Piece object
 * @return A SlimOptional<Piece> object representing the piece,
 * or a nullopt if the character does not represent a valid piece or represents an empty square
 */
constexpr inline SlimOptional<Piece> pieceFromChar(char ch) noexcept {
    switch (ch) {
        case 'k':
            return SlimOptional(Piece(Team::BLACK, PieceType::KING));
        case 'K':
            return SlimOptional(Piece(Team::WHITE, PieceType::KING));
        case 'q':
            return SlimOptional(Piece(Team::BLACK, PieceType::QUEEN));
        case 'Q':
            return SlimOptional(Piece(Team::WHITE, PieceType::QUEEN));
        case 'b':
            return SlimOptional(Piece(Team::BLACK, PieceType::BISHOP));
        case 'B':
            return SlimOptional(Piece(Team::WHITE, PieceType::BISHOP));
        case 'r':
            return SlimOptional(Piece(Team::BLACK, PieceType::CASTLE));
        case 'R':
            return SlimOptional(Piece(Team::WHITE, PieceType::CASTLE));
        case 'n':
            return SlimOptional(Piece(Team::BLACK, PieceType::KNIGHT));
        case 'N':
            return SlimOptional(Piece(Team::WHITE, PieceType::KNIGHT));
        case 'p':
            return SlimOptional(Piece(Team::BLACK, PieceType::PAWN));
        case 'P':
            return SlimOptional(Piece(Team::WHITE, PieceType::PAWN));
        default:
            return SlimOptional<Piece>::nullopt();
    }
}

/**
 * @brief Converts a character to an integer
 *
 * @param ch The character to convert to an integer
 * @return The integer value of the digit represented by the input character
 */
constexpr inline uint8_t charToInt(char ch) noexcept {
    return ch - '0';
}

/**
 * @brief Converts a chess notation square to a position in in the Board
 *
 * @param character The character representing the column of the square
 * @param number The character representing the row of the square
 *
 * @return The position in the Board class representing the chess notation square if it is a valid square,
 * otherwise returns a nullopt
 */
constexpr inline ChessPos getPosFromChar(char character, char number) {
    if (character < 'a' || character > 'h' || number < '1' || number > '8') return ChessPos::nullopt();

    uint8_t row = character - 'a';
    uint8_t column = charToInt(number);

    int8_t res = static_cast<int8_t>((8 - column) * 8 + row);
    return SlimOptional(res);
}

/**
 * @brief Converts a string to a User made Move \n \n
 *
 * The string is expected to represent a move in a chess game, in the format "[Piece][Source][Capture][Target][Promotion]".
 * (the same format as in the exercises)
 *
 * @param input The string to convert to a User made Move
 * @return An Optional User move representing the move, or a nullopt if the string does not represent a valid move
 *
 *@example valid moves: \n
 * normal move: Rh1f1, \n
 * capture: Pe4xf5, \n
 * promotion : Pa7a8=Q \n
 * castling: Ke1c1 \n
 */
constexpr SlimOptional<UserMove> convert_string_to_move(std::string_view input) {
    // check no out of bounds access necessary
    if (input.size() < 3) return SlimOptional<UserMove>::nullopt();

    UserMove move = UserMove();

    // check Piece
    SlimOptional<Piece> piece = pieceFromChar(input[0]);
    if (!piece.has_value()) return SlimOptional<UserMove>::nullopt();
    move.piece = piece.data;

    // check source position
    ChessPos pos = getPosFromChar(input[1], input[2]);
    if (!pos.has_value()) return SlimOptional<UserMove>::nullopt();
    move.from = pos.data;

    // check capture
    move.is_capture = input[3] == 'x';

    // check no out of bounds access necessary 2
    if (input.size() < 5 + move.is_capture) return SlimOptional<UserMove>::nullopt();

    // check target position
    pos = getPosFromChar(input[3 + move.is_capture], input[4 + move.is_capture]);

    if (!pos.has_value()) return SlimOptional<UserMove>::nullopt();
    move.to = pos.data;

    // check promotion
    if (input[input.size() - 2] == '=') {
        SlimOptional<Piece> promote = pieceFromChar(input[input.size() - 1]);
        if (!promote.has_value()) return SlimOptional<UserMove>::nullopt();
        if (promote.data.team != piece.data.team) return SlimOptional<UserMove>::nullopt();
        if (promote.data.type == PieceType::PAWN || promote.data.type == PieceType::KING) return SlimOptional<UserMove>::nullopt();

        move.promote.data = promote.data.type;
    }

    return SlimOptional(move);
}