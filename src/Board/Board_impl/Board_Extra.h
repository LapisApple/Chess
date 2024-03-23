//
// Created by timap on 27.02.2024.
//

#pragma once
#include "../../Types/BasicChessTypes.h"
#include "../../Types/SlimOptional.h"
#include "../../Types/Move/Move.h"
#include "../../ChessConstants.h"

/* castling:
 * so basically the idea is to use bit flags to represent the right to castle in a certain direction
 * each team gets 2 following bits with the left bit being castling queen side and the right being castle right king side
 * that's also why the specialMoves around castling have these magic values
 */

/**
 * @brief a simple and memory efficient data structure to represent castling and en Passant in Chess
 */
class Board_Extra {
private:
    /**
     * amount to shift castling rights for the black team
     */
    constexpr static inline uint8_t black_shift = Team::BLACK << 1;
    /**
     * amount to shift castling rights for the white team
     */
    constexpr static inline uint8_t white_shift = Team::WHITE << 1;

    /**
     * castling flags stored in the lower 4 bits of an 8-bit unsigned integer
     */
    uint8_t castling;
    /**
     * possible en Passant Position stored in a Custom Optional
     */
    SlimOptional<int8_t> passant_pos;

// private setters
    /**
     * @brief internal method to disable certain castling flags for a specific team
     * @param team the team for which to disable some castling flags (undefined for TEAM::NONE)
     * @param specialMove_losing_castling_right the castling flags to disable not shifted by the team amount
     * defined for the inputs: 0b00, 0b01, 0b10, 0b11, undefined for all other inputs (flags can just directly be taken from the corresponding SpecialMove enums)
     */
    constexpr void loseCastlingRight(Team::Team team, uint8_t specialMove_losing_castling_right) noexcept {
#ifndef NO_ASSERTS
        assert( team == Team::WHITE || team == Team::BLACK);
        assert(specialMove_losing_castling_right <= 0b11);
#endif
        const uint8_t lost_castling_right_bits = specialMove_losing_castling_right << (team << 1);
        const uint8_t keep_mask = ~lost_castling_right_bits;
        castling &= keep_mask;
    }

public:
// constructors
    /**
     * @brief constructs Board_Extra with the values of the starting game position
     * meaning no en Passant Position and all four castling rights
     */
    explicit constexpr Board_Extra() noexcept : castling(0b1111), passant_pos(ChessPos::nullopt()) {
    }

    /**
     * @brief constructs Board_Extra with the given parameters
     * @param castling_right_flags the castling rights as flags in a 8-bit unsigned integer
     * @param en_passant_pos an Optional of the possible En Passant Position (sth. like the "shadow" of where it can be taken)
     * @note This constructor does not check if the the given arguments are actually legal ,
     * but expects them to be supplied by the Fen class, which checks for that legality
     */
    explicit constexpr Board_Extra(uint8_t castling_right_flags, ChessPos en_passant_pos) noexcept : castling(castling_right_flags), passant_pos(en_passant_pos) {
    }

// getters
    /**
     * @brief a simple abstraction to get the castling rights for the board
     * @param pos the position the piece that should have something to do with castling rights
     * meaning basically the starting positions for the king and castles
     * @return the castling flags that have something to do with the piece on that position
     * @note can also be used like a condition due to the return being 0 when there are no castling flags for that position
     * and >0 if there are castling flags
     */
    [[nodiscard]] constexpr uint8_t getCastlingRights(int8_t pos) const noexcept {
        uint8_t mask = 0b0;
        switch (pos) {
            case ChessConstants::start_black_queen_side_castle_pos:
                mask = 0b10 << black_shift;
                break;
            case ChessConstants::start_black_king_pos:
                mask = 0b11 << black_shift;
                break;
            case ChessConstants::start_black_king_side_castle_pos:
                mask = 0b01 << black_shift;
                break;
            case ChessConstants::start_white_queen_side_castle_pos:
                mask = 0b10 << white_shift;
                break;
            case ChessConstants::start_white_king_pos:
                mask = 0b11 << white_shift;
                break;
            case ChessConstants::start_white_king_side_castle_pos:
                mask = 0b01 << white_shift;
                break;
            default:
                break;
        }
        return castling & mask;
    }

    /**
     * @brief access to en passant position without the ability to modify it
     * @return a custom Optional of the en passant position
     * @note en passant position is a different field on the board from the en passantable piece. to visualize it,
     * this position is basically like the "shadow" of the en passantable piece left while he was moving 2 fields
     */
    [[nodiscard]] constexpr ChessPos getPassantPos() const noexcept {
        return passant_pos;
    }

    /**
     * @brief calculating the en passantable piece position
     * @return a custom Optional of the en passantable position with a nullopt if there is no en passantable piece
     * @note en passantable piece position is a different field on the board from en passant position.
     * en Passantable Piece position is the actual position of where the pawn, that can be taken by en passant, is located
     */
    [[nodiscard]] constexpr ChessPos getPosOfPassantablePiece() const noexcept {
        if (!passant_pos.has_value()) return ChessPos::nullopt();

        const int8_t row = static_cast<int8_t>(passant_pos.data >> 3);
        const int8_t offset = row == ChessConstants::black_en_passant_row ? 8 : -8;
#ifndef NO_ASSERTS
        constexpr int ROW_ERROR_VALUE = -1;
        const int detailed_offset = row == ChessConstants::black_en_passant_row ? 8 : ( (row == ChessConstants::white_en_passant_row) ? -8 : ROW_ERROR_VALUE);
        assert(detailed_offset != ROW_ERROR_VALUE);
        assert(offset == detailed_offset);
#endif
        return SlimOptional(static_cast<int8_t>(passant_pos.data + offset));
    }

// setters
    /**
     * @brief "moves" a piece and updates any needed values
     * @param move the move to be executed
     * @param piece_captured the piece that is being captured, or overwritten if none is captured, by the current move
     * @note does not check if the move and/or piece_captured are actually valid
     */
    constexpr void movePiece(Move move, Piece piece_captured) noexcept {
        passant_pos = ChessPos::nullopt();

        switch (move.specialMove.data) {
            case SpecialMove::loseCastlingKingSide:
            case SpecialMove::loseCastlingQueenSide:
            case SpecialMove::loseCastlingBoth:
                loseCastlingRight(move.piece.team, static_cast<uint8_t>(move.specialMove.data));
                break;
            case SpecialMove::pawnMove2:
                passant_pos = SlimOptional(static_cast<int8_t>(move.from + (move.to - move.from) / 2));
                break;
            case SpecialMove::CastleKingSide:
            case SpecialMove::CastleQueenSide:
                loseCastlingRight(move.piece.team, static_cast<uint8_t>(SpecialMove::loseCastlingBoth));
                break;
            default:
                break;
        }

        // don't let king castle with already taken castle
        if (piece_captured.type == PieceType::CASTLE) {
            const uint8_t lost_flags = this->getCastlingRights(move.to);
            // this should be working. it hasn't produced any problems in perft
            // explanation (to past self):
            // works because the return of getCastlingRights() only has '1' on bits that are currently on in the castling variable
            // meaning that this xor turns of the correct bits and does not turn any back on
            castling ^= lost_flags;
        }
    }
};
