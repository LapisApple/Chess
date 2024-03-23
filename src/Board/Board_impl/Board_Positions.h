//
// Created by timap on 27.01.2024.
//

#pragma once
#include <cstdint>
#include <cassert>

#include "../../Types/BasicChessTypes.h"
#include "Board_8x8.h"
#include "../../Defines.h"


/**
 * @brief a memory efficient data structure with high memory locality
 * representing the Chess Board as lists of various Pieces containing that Piece's positions
 */
class Board_Positions {
private:
    /**
     * @brief a 2D array of each Pieces boundary with the next piece, meaning the first field after the end of the current PieceType
     * @note ends[][0] are the delimiter for the PieceType none meaning they should always be 0
     */
    int8_t ends[TEAM_AMOUNT][PIECETYPE_AMOUNT] = {0};
    /**
     * @brief a 2D array of each Pieces positions
     * @note the data in ends[][] is needed to get the actual positions for a specific piece as a list \n
     * (empty position as set to 0 for convenience, but they should not be accessible outside of this class)
     */
    int8_t positions[TEAM_AMOUNT][PIECES_PER_TEAM] = {0};

private:
// private getters
    /**
     * @brief a simple helper method to get the index of a certain position on a certain field
     * @param piece the piece to be searched for (to limit the search range)
     * @param pos the position where the specified piece is located
     *
     * @return an Optional of the index in the positions[piece.team] array
     */
    [[nodiscard]] constexpr ChessPos getPositionIndex(Piece piece, int8_t pos) const noexcept {
        for (int8_t pos_idx = ends[piece.team][piece.type - 1]; pos_idx < ends[piece.team][piece.type]; ++pos_idx) {
            if (positions[piece.team][pos_idx] == pos) return SlimOptional(pos_idx);
        }
        return ChessPos::nullopt();
    }

// private worse memmove impl.
    // because memmove is not defined when working with overlapping regions of memory, which was really unexpected,
    // because I thought that was the reason why you should use memmove instead of memcpy in such situations
    /**
     * @brief a worse implementation of memmove for moving memory left,
     * because memmove is unexpectedly undefined with overlapping regions of memory
     * @note arguments should have the same effect as normal memmove, but (dst < src) should be true
     */
    static constexpr void worseMemmove_left(int8_t* dst, const int8_t* src, int amount) noexcept  {
#ifndef NO_ASSERTS
        assert(dst < src);
#endif
        for (int i = 0; i < amount; ++i) {
            dst[i] = src[i];
        }
    }
    /**
     * @brief a worse implementation of memmove for moving memory right,
     * because memmove is unexpectedly undefined with overlapping regions of memory
     * @note arguments should have the same effect as normal memmove, but (dst > src) should be true
     */
    static constexpr void worseMemmove_right(int8_t* dst, const int8_t* src, int amount) noexcept {
#ifndef NO_ASSERTS
        assert(dst > src);
#endif
        for (int i = amount - 1; i >= 0; --i) {
            dst[i] = src[i];
        }
    }

// shifting boundaries
    /**
    * @brief a method to shift the boundaries of where a pieceType ends by a certain amount
     * @param team the team where the shifting should take place
     * @param begin_type the pieceType at which the search for shifting should start
     * @param end_type the pieceType at which the search for shifting should end
     * @param lhs a lower bound on the boundary value for the shifting process
     * @param rhs the largest boundary value to be shifted for the shifting process
     * @param dir the amount and direction of shifting being done
     *
     * in the range start_type=..\<end_type changes all entries of ends[team] in the exclusive range lhs\<..=rhs by dir
     *
     * @note ranges: \n
     * start_type=..\<end_type \n
     * lhs\<..=rhs
    */
    constexpr inline void shiftBoundaries_impl(Team::Team team,int begin_type,int end_type, int lhs, int rhs, int8_t dir) noexcept {
        for (int pieceType = begin_type; pieceType < end_type; ++pieceType) {
            const int8_t current_piece_end = ends[team][pieceType];
            if (current_piece_end > lhs && current_piece_end <= rhs) {
                ends[team][pieceType] = static_cast<int8_t>(current_piece_end + dir);
            }
        }
    }

    /**
    * @brief wrapper around shiftBoundaries_impl() with the values needed for capture
     * @param team the team where the shifting should take place
     * @param lhs a lower bound on the boundary value for the shifting process
     * @param rhs the largest boundary value to be shifted for the shifting process
     *
     * changes all entries of ends[team] in the exclusive range lhs\<..=rhs by dir
     *
     * @note ranges: \n
     * lhs\<..=rhs
    */
    constexpr inline void shiftBoundariesCapture(Team::Team team, int lhs, int rhs) noexcept {
        shiftBoundaries_impl(team, 0, PIECETYPE_AMOUNT, lhs, rhs, -1);
    }

    /**
    * @brief wrapper around shiftBoundaries_impl() with the values needed for promotion
     * @param team the team where the shifting should take place
     * @param lhs a lower bound on the boundary value for the shifting process
     * @param rhs the largest boundary value to be shifted for the shifting process
     * @param promote_to the pieceType that the pawn is promoting to
     *
     * in the needed range for promotion (promote_to=..\<PieceType::PAWN) \n
     * changes all entries of ends[team] in the exclusive range lhs\<..=rhs by dir
     *
     * @note ranges: \n
     * lhs\<..=rhs
    */
    constexpr inline void shiftBoundariesPromotion(Team::Team team, PieceType::PieceType promote_to, int lhs, int rhs) noexcept {
        shiftBoundaries_impl(team, promote_to, PieceType::PAWN, lhs, rhs, 1);
    }

    /**
    * @brief wrapper around shiftBoundaries_impl() with the values needed for bringing back a captured piece
     * @param team the team of the captured piece
     * @param type the type of the captured piece
     * @param lhs a lower bound on the boundary value for the shifting process
     * @param rhs the largest boundary value to be shifted for the shifting process
     *
     * in the needed range for reanimation (type=..\<=PieceType::PAWN) \n
     * changes all entries of ends[team] in the exclusive range lhs\<..=rhs by dir
     *
     * @note ranges: \n
     * lhs\<..=rhs
    */
    constexpr inline void shiftBoundariesReanimate(Team::Team team,PieceType::PieceType type, int lhs, int rhs) noexcept {
        shiftBoundaries_impl(team, type, PIECETYPE_AMOUNT, lhs, rhs, 1);
    }

    /**
    * @brief wrapper around shiftBoundaries_impl() with the values needed for promotion
     * @param team the team of the promoted piece
     * @param team the type that the piece previously promoted to
     * @param lhs a lower bound on the boundary value for the shifting process
     * @param rhs the largest boundary value to be shifted for the shifting process
     *
     * in the needed range for promotion (type=..\<PieceType::PAWN) \n
     * changes all entries of ends[team] in the exclusive range lhs\<..=rhs by dir
     *
     * @note ranges: \n
     * lhs\<..=rhs
    */
    constexpr inline void shiftBoundariesDemotion(Team::Team team,PieceType::PieceType type, int lhs, int rhs) noexcept {
        shiftBoundaries_impl(team, type, PieceType::PAWN, lhs, rhs, -1);
    }

// handling special move abilities
    /**
     * @brief promotes a piece at an index to a specified Piece
     * @param team_to_promote the team of the piece to be promoted
     * @param promote_to the pieceType the Pawn promotes to
     * @param old_position_idx the index in positions[][] that the pawn currently occupies
     *
     * After Capture, Updates the class to keep Equality with the grid representation
     */
    constexpr inline void promoteToPiece(Team::Team team_to_promote, PieceType::PieceType promote_to, int old_position_idx) noexcept {
        const int8_t piece_pos = positions[team_to_promote][old_position_idx];
        const int8_t new_position_idx = ends[team_to_promote][promote_to];
        const int amount_to_shift = old_position_idx - new_position_idx;

        int8_t* new_position_ptr = positions[team_to_promote] + new_position_idx;
        worseMemmove_right(new_position_ptr + 1, new_position_ptr, amount_to_shift);
        // std::memmove(new_position_ptr + 1, new_position_ptr, amount_to_shift);
        positions[team_to_promote][new_position_idx] = piece_pos;

        shiftBoundariesPromotion(team_to_promote, promote_to, new_position_idx - 1, old_position_idx + 1);
    }

    /**
     * @brief captures a piece at a specific index
     * @param team_to_capture the team of the piece to be captured
     * @param capture_piece_idx the index in positions[][] that the to be captured piece currently occupies
     *
     * After Capture, Updates the class to keep Equality with the grid representation
     */
    constexpr inline void capturePiece(Team::Team team_to_capture, int capture_piece_idx) noexcept {
        const int8_t last_end = ends[team_to_capture][PIECETYPE_AMOUNT - 1];
        const int last_piece_idx = last_end - 1;
        const int amount_to_shift = last_piece_idx - capture_piece_idx;

        int8_t* capture_position_ptr = positions[team_to_capture] + capture_piece_idx;
        worseMemmove_left(capture_position_ptr, capture_position_ptr + 1, amount_to_shift);
        // std::memmove(capture_position_ptr, capture_position_ptr + 1, amount_to_shift);
        positions[team_to_capture][last_piece_idx] = 0;

        shiftBoundariesCapture(team_to_capture, capture_piece_idx, last_end);
    }

    /**
     * @brief demotes a previously promoted Pawn back to being a Pawn
     * @param team_to_demote the team of the piece to be demoted
     * @param demote_from the piece the pawn had previously promoted to
     * @param piece_position_idx the index in positions[][] that the promoted piece currently occupies
     */
    constexpr inline void demotePiece(Team::Team team_to_demote, PieceType::PieceType demote_from, int piece_position_idx) noexcept {
        const int8_t piece_pos = positions[team_to_demote][piece_position_idx];
        const int new_pos_idx = ends[team_to_demote][PIECETYPE_AMOUNT - 1] - 1;

        const int amount_to_shift = new_pos_idx - piece_position_idx; // +x ?

        int8_t* position_ptr = positions[team_to_demote] + piece_position_idx;
        worseMemmove_left(position_ptr, position_ptr + 1, amount_to_shift);
        positions[team_to_demote][new_pos_idx] = piece_pos;

        shiftBoundariesDemotion(team_to_demote, demote_from, piece_position_idx, new_pos_idx + 1);
    }

    /**
     * @brief Reanimates a specific piece that was previously captured
     * @param team_of_capture the team of the to be reanimated piece
     * @param type the type of the to be reanimated piece
     * @param piece_pos the position that the previously captured piece occupied (on which square it should be reanimated)
     */
    constexpr inline void reanimatePiece(Team::Team team_of_capture, PieceType::PieceType type, int8_t piece_pos) noexcept {
        const int8_t last_end_idx = ends[team_of_capture][PIECETYPE_AMOUNT - 1];
        const int8_t reanimate_idx = ends[team_of_capture][type];

        const int amount_to_shift = last_end_idx - reanimate_idx; // +1 ?

        int8_t* reanimate_position_ptr = positions[team_of_capture] + reanimate_idx;
        worseMemmove_right(reanimate_position_ptr + 1, reanimate_position_ptr, amount_to_shift);

        positions[team_of_capture][reanimate_idx] = piece_pos;

        shiftBoundariesReanimate(team_of_capture, type, reanimate_idx - 1, last_end_idx);
    }


public:
// constructors
    /**
     * @brief constructs this class with the start Chess board state
     */
    constexpr explicit Board_Positions() noexcept: Board_Positions(Board_8x8()) {}

    /**
     * @brief constructs the Board_Positions through the Board Grid representation
     * @param board_grid the board state to be converted to an equal representation as Piece Lists
     */
    constexpr explicit Board_Positions(const Board_8x8& board_grid) noexcept {

        // setup separators
        uint8_t amount_pieceType[TEAM_AMOUNT][PIECETYPE_AMOUNT] = {0};
        for (int i = 0; i < BOARD_SIZE; ++i) {
            Piece piece = board_grid[i];
            if (piece.team == Team::NONE) continue;
            amount_pieceType[piece.team][piece.type]++;
        }

#ifndef NO_ASSERTS
     // asserts that a possible amount of pieces is found
        for (auto& pieceType1Team : amount_pieceType) {
            int sum = 0;
            for (uint8_t amountPieces : pieceType1Team) {
                sum += amountPieces;
            }
            assert(sum <= PIECES_PER_TEAM);
        }
#endif

        for (int i = 0; i < TEAM_AMOUNT; ++i) {
            ends[i][0] = 0;
            for (int j = 1; j < PIECETYPE_AMOUNT; ++j) {
                ends[i][j] = static_cast<int8_t>(ends[i][j - 1] + amount_pieceType[i][j]);
            }
        }

#ifndef NO_ASSERTS
     // basically asserts that there should not have been illegal pieces
     // and each team should not have more pieces than possible in chess
        for (auto & end1Team : ends) {
            assert(end1Team[PieceType::NONE] == 0);
            assert(end1Team[PIECETYPE_AMOUNT - 1] <= PIECES_PER_TEAM);
        }
#endif

        // setup positions
        for (int8_t i = 0; i < BOARD_SIZE; ++i) {
            Piece piece = board_grid[i];
            if (piece.type == PieceType::NONE) continue;

            uint8_t piece_offset = ends[piece.team][piece.type] - amount_pieceType[piece.team][piece.type];
            positions[piece.team][piece_offset] = i;
            amount_pieceType[piece.team][piece.type]--;
        }

#ifndef NO_ASSERTS
     // basically asserts that all pieces found previously were used
        for (auto& amountPieceType1Team : amount_pieceType) {
            for (int j = 1; j < PIECETYPE_AMOUNT; ++j) {
                assert(amountPieceType1Team[j] == 0);
            }
        }
#endif
    }

// subclass
    /**
     * @brief a simple Slice of the Position array as a List of Positions for a specific Piece
     */
    struct PositionsSlice {
    private:
        /**
        * @brief the start of the position list
        */
        const int8_t* data = nullptr;
    public:
        /**
        * @brief the amount of positions in the list
        */
        const int size = 0;

        /**
        * @brief a simple constructor
         * @param size the amount of positions in the list
         * @param data a pointer to the start of the list
         *
         * @note does not check if the list is actually valid, meaning might SegFault when given wrong arguments
        */
        constexpr inline PositionsSlice(int size, const int8_t* data) noexcept: size(size), data(data) {}

        /**
        * @brief a simple direct access operator
         * @param i the index of the item to be accessed
         *
     * @note bounds checks can be disabled by defining NO_BOUNDS_CHECKS
        */
        constexpr inline int8_t operator[](int i) const noexcept {
#ifndef NO_BOUNDS_CHECKS
            assert(i >= 0 && i < size);
#endif
            return data[i];
        };
    };

// getters
    /**
     * @brief "creates" a list of Positions for the specified Piece
     * @param team the team of the piece for the List "creation"
     * @param type the pieceType of the piece for the List "creation"
     * @return A PositionsSlice as a List of Position of the specified Piece
     *
     * @note bounds checks can be disabled by defining NO_BOUNDS_CHECKS \n
     * \n
     * Furthermore, this function should not be with Team::NONE or PieceType::NONE
     */
    [[nodiscard]] constexpr PositionsSlice getVec(Team::Team team, PieceType::PieceType type) const noexcept {
#ifndef NO_ASSERTS
        assert(team == Team::WHITE || team == Team::BLACK);
        assert(type != PieceType::NONE);
#endif
        const int8_t start_idx = ends[team][type - 1];
        const int8_t end_idx = ends[team][type];

#ifndef NO_BOUNDS_CHECKS
        assert(end_idx <= 16);
        assert(start_idx >= 0);
        assert(start_idx <= end_idx);
#endif
        const int size = end_idx - start_idx;
        const int8_t* data = &(positions[team][start_idx]);

        return PositionsSlice(size, data);
    }

    /**
     * @brief calculates the amount of times a certain Piece exists
     * @param team the team of the piece for the calculation of the amount
     * @param type the pieceType of the piece for the calculation of the amount
     * @return the number of times this specific Piece exists
     *
     * @note Calling this function with Team::NONE or PieceType::NONE is undefined behaviour
     */
    [[nodiscard]] constexpr int getAmount(Team::Team team, PieceType::PieceType type) const noexcept {
#ifndef NO_ASSERTS
        assert(team == Team::WHITE || team == Team::BLACK);
        assert(type != PieceType::NONE);
#endif
        const int8_t start_idx = ends[team][type - 1];
        const int8_t end_idx = ends[team][type];

#ifndef NO_ASSERTS
        assert(end_idx <= 16);
        assert(start_idx >= 0);
        assert(start_idx <= end_idx);
#endif
        const int amount = end_idx - start_idx;

        return amount;
    }

    /**
     * @brief function to test for existence of a specific Piece on the board
     * @param team the team of the piece for the calculation
     * @param type the pieceType of the piece for the calculation
     * @return whether such a Piece exists on the Board
     *
     * @note Calling this function with Team::NONE or PieceType::NONE is undefined behaviour
     */
    [[nodiscard]] constexpr inline bool hasPiece(Team::Team team, PieceType::PieceType type) const noexcept {
#ifndef NO_ASSERTS
        assert(team == Team::WHITE || team == Team::BLACK);
        assert(type != PieceType::NONE);
#endif
        return ends[team][type] > ends[team][type - 1];
    }


    /**
     * @brief access to the position of the king
     * @param team the team of the king
     * @return the position of that king
     *
     * @note Calling this function while team does not have a king is undefined behaviour
     */
    [[nodiscard]] constexpr int8_t getKingPos(Team::Team team) const noexcept {
#ifndef NO_ASSERTS
        assert(hasPiece(team, PieceType::KING));
#endif
        return positions[team][0];
    }

// setters
    /**
     * @brief executes the given move and updates any needed values
     * @param move the move to be executed
     * @param capture an optional of the captured Piece
     * @param pos_of_passantable_pawn a custom Optional of the actual position of the piece that would be taken by en passant
     * @note does not check if the move, capture, and pos_of_passantable_pawn piece are actually valid
     */
    constexpr void movePiece(Move move, SlimOptional<Piece> capture, ChessPos pos_of_passantable_piece) {
        const ChessPos fromIndex = getPositionIndex(move.piece, move.from);
#ifndef NO_ASSERTS
        assert(fromIndex.has_value());
#endif
        positions[move.piece.team][fromIndex.data] = move.to;

        switch (move.specialMove.data) {
            case SpecialMove::CastleKingSide:
            {
                const int8_t castle_pos = static_cast<int8_t>(move.from + 3);
                const ChessPos castle_pos_idx = getPositionIndex(Piece(move.piece.team, PieceType::CASTLE), castle_pos);
#ifndef NO_ASSERTS
                assert(castle_pos_idx.has_value());
#endif
                positions[move.piece.team][castle_pos_idx.data] = static_cast<int8_t>(move.from + 1);
            }
                break;
            case SpecialMove::CastleQueenSide:
            {
                const int8_t castle_pos = static_cast<int8_t>(move.from - 4);
                const ChessPos castle_pos_idx = getPositionIndex(Piece(move.piece.team, PieceType::CASTLE), castle_pos);
#ifndef NO_ASSERTS
                assert(castle_pos_idx.has_value());
#endif
                positions[move.piece.team][castle_pos_idx.data] = static_cast<int8_t>(move.from - 1);
            }
                break;

            default:
                break;
        }

        if (capture.has_value()) {
            const bool is_en_passant_capture = move.specialMove.data == SpecialMove::en_Passant;
#ifndef NO_ASSERTS
            if (is_en_passant_capture) assert(pos_of_passantable_piece.has_value());
#endif
            const int8_t capturePos = is_en_passant_capture ? pos_of_passantable_piece.data : move.to;
            const ChessPos captureIndex = getPositionIndex(capture.data, capturePos);
#ifndef NO_ASSERTS
            assert(captureIndex.has_value());
#endif
            capturePiece(Team::getEnemyTeam(move.piece.team), captureIndex.data);
        }

        if (move.promote.has_value()) {
            promoteToPiece(move.piece.team, move.promote.data, fromIndex.data);
        }

    }

    /**
     * @brief undoes a previously executed move and updates any needed values
     * @param move the move to be undone
     *
     * @note the move to be undone needs to be the previously executed move
     */
    constexpr void undoMove(ExecutedMove move) {
        Piece piece_after_move = move.move.piece;
        if (move.move.promote.has_value()) piece_after_move.type = move.move.promote.data;

        const ChessPos to_idx = getPositionIndex(piece_after_move, move.move.to);

#ifndef NO_ASSERTS
        assert(to_idx.has_value());
#endif
        positions[piece_after_move.team][to_idx.data] = move.move.from;

        switch (move.move.specialMove.data) {
            case SpecialMove::CastleKingSide:
            {
                const int8_t castle_pos = static_cast<int8_t>(move.move.from + 1);
                const ChessPos castle_pos_idx = getPositionIndex(Piece(piece_after_move.team, PieceType::CASTLE), castle_pos);
#ifndef NO_ASSERTS
                assert(castle_pos_idx.has_value());
#endif
                positions[piece_after_move.team][castle_pos_idx.data] = static_cast<int8_t>(move.move.from + 3);
            }
                break;
            case SpecialMove::CastleQueenSide:
            {
                const int8_t castle_pos = static_cast<int8_t>(move.move.from - 1);
                const ChessPos castle_pos_idx = getPositionIndex(Piece(piece_after_move.team, PieceType::CASTLE), castle_pos);
#ifndef NO_ASSERTS
                assert(castle_pos_idx.has_value());
#endif
                positions[piece_after_move.team][castle_pos_idx.data] = static_cast<int8_t>(move.move.from - 4);
            }
                break;

            default:
                break;
        }


        if (move.capture.has_value()) {
            const bool is_en_passant_capture = move.move.specialMove.data == SpecialMove::en_Passant;
            const ChessPos pos_of_passantable_piece = move.extra.getPosOfPassantablePiece();
#ifndef NO_ASSERTS
            if (is_en_passant_capture) assert(pos_of_passantable_piece.has_value());
#endif
            const int8_t capturePos = is_en_passant_capture ? pos_of_passantable_piece.data : move.move.to;
            reanimatePiece(move.capture.data.team, move.capture.data.type, capturePos);
        }

        if (move.move.promote.has_value()) {
            demotePiece(piece_after_move.team, piece_after_move.type, to_idx.data);
        }
    }
};

