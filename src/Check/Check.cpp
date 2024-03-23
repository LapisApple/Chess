//
// Created by timap on 27.01.2024.
//

#include "Check.h"
#include "../Types/Vec2.h"

/**
* @brief the amount of Positions a knight can attack a king from
*/
#define AMOUNT_KNIGHT_OFFSET 8

/**
* @brief the amount of Positions a pawn can attack a king from
*/
#define AMOUNT_PAWN_OFFSET 2

/**
* @brief the amount of Positions a king can attack a king from
*/
#define AMOUNT_KING_OFFSET 8

/**
* @brief the amount of directions that a bishop and castle can each move in
*/
#define AMOUNT_HALF_DIRECTIONS 4

/**
 * @brief Checks if the king is in check from non sliding pieces
 *
 * @param board The current state of the chess board
 * @param offset_list The list of offsets to check, where an attacking piece could be located
 * @param offset_amount The amount of offsets in the offset_list
 * @param king_pos The position of the king
 * @param enemy_piece The type of enemy piece to check for
 * @return true if the king is being attacked by the specified enemy piece (i.e. the king is in Check), false otherwise
 */
static constexpr bool king_check_offsets(const Board& board, const Vec2* offset_list, int offset_amount, Vec2 king_pos, Piece enemy_piece) noexcept {
    for (int i = 0; i < offset_amount; ++i) {
        const Vec2 current_offset = offset_list[i];
        const Vec2 current_pos = king_pos + current_offset;
        if (!current_pos.outsideBoard() && board.grid[current_pos.getPos()] == enemy_piece) return true;
    }

    return false;
}

/**
 * @brief Checks if the king is in check from sliding pieces
 *
 * @param board The current state of the chess board
 * @param direction_list The list of directions to check, where an attacking piece could be located
 * @param king_pos The position of the king
 * @param team The team of the king (used to check for enemy pieces)
 * @param pieceType the pieceType that can move in such directions
 * (not including queen, because queen is implicitly checked at the same time)
 * @return true if the king is being attacked by the specified enemy piece (i.e. the king is in Check), false otherwise
 */
static constexpr bool king_check_linear_search(const Board& board, const Vec2* direction_list, Vec2 king_pos, Team::Team team,  PieceType::PieceType pieceType) noexcept {
    for (int i = 0; i < AMOUNT_HALF_DIRECTIONS; ++i) {
        const Vec2 dir = direction_list[i];

        for (Vec2 current = king_pos + dir; !current.outsideBoard(); current += dir) {
            const Piece currentPiece = board.grid[current.getPos()];

            if (currentPiece.team == Team::NONE) continue;

            if ((currentPiece.team != team) && (currentPiece.type == pieceType || currentPiece.type == PieceType::QUEEN )) return true;
            break;
        }
    }

    return false;
}

// (function documentation is provided in the corresponding header)
bool Check::isChecked(const Board& board, Team::Team team, int8_t king_pos) noexcept {
    const Vec2 king_pos_2D = Vec2::newVec2(king_pos);

    const Team::Team enemy_team = Team::getEnemyTeam(team);
    bool is_check = false;

    // knight test
    if (board.positions.hasPiece(enemy_team, PieceType::KNIGHT)) {
        constexpr Vec2 knight_offset_list[AMOUNT_KNIGHT_OFFSET] = {{-2, -1}, {-2, 1}, {2, -1}, {2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}};
        is_check = king_check_offsets(board, knight_offset_list, AMOUNT_KNIGHT_OFFSET, king_pos_2D, Piece(enemy_team, PieceType::KNIGHT));
        if (is_check) return true;
    }

    // pawn test
    if (board.positions.hasPiece(enemy_team, PieceType::PAWN)) {
        const int8_t pawnDir = team == Team::WHITE ? -1 : 1;
        const Vec2 pawn_offset_list[AMOUNT_PAWN_OFFSET] = {{-1, pawnDir}, {1, pawnDir}};
        is_check = king_check_offsets(board, pawn_offset_list, AMOUNT_PAWN_OFFSET, king_pos_2D, Piece(enemy_team, PieceType::PAWN));
        if (is_check) return true;
    }

    // king test
    constexpr Vec2 king_offset_list[AMOUNT_KING_OFFSET] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}, {0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    is_check = king_check_offsets(board, king_offset_list, AMOUNT_KING_OFFSET, king_pos_2D, Piece(enemy_team, PieceType::KING));
    if (is_check) return true;

    // Bishop + 1/2 QUEEN
    if (board.positions.hasPiece(enemy_team, PieceType::BISHOP) || board.positions.hasPiece(enemy_team, PieceType::QUEEN)) {
        constexpr Vec2 diagonal_steps[AMOUNT_HALF_DIRECTIONS] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
        is_check = king_check_linear_search(board, diagonal_steps, king_pos_2D, team, PieceType::BISHOP);
        if (is_check) return true;
    }

    // CASTLE + 1/2 QUEEN
    if (board.positions.hasPiece(enemy_team, PieceType::CASTLE) || board.positions.hasPiece(enemy_team, PieceType::QUEEN)) {
        constexpr Vec2 linear_steps[AMOUNT_HALF_DIRECTIONS] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
        is_check = king_check_linear_search(board, linear_steps, king_pos_2D, team, PieceType::CASTLE);
        if (is_check) return true;
    }

    return false;
}