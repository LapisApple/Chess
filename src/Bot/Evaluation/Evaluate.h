//
// Created by timap on 06.03.2024.
//

#pragma once

#include "../../Board/Board.h"
#include "../../Types/Vec2.h"
#include "pesto_tables.h"

/**
 * @brief Maximum percentage for both game phase percentage values.
 *
 * @note This constant is used to cap the game phase percentage to 24.
 */
constexpr int max_phase_percentage = 24;
/**
 * @brief Advantage for the player who is allowed to move next.
 */
constexpr int movers_advantage = 15;
/**
 * @brief Factor for each piece that is next to and protecting the king
 */
constexpr int protected_king_factor = 2;



/**
 * @brief Struct to hold the board value for midgame and endgame.
 *
 * @note
 * mg = the midgame value \n
 * eg = the endgame value
 */
struct BoardValue {
    int mg = 0;
    int eg = 0;
};

/**
 * @brief Calculates the piece values for a given team \n
 * and increases the mid game percentage (i.e. how much the board is still in the midgame).
 *
 * @param board The current board state
 * @param team The team for which to calculate the piece values
 * @param mid_game_percentage A reference to the game phase variable to be updated
 * @return A BoardValue struct containing the midgame and endgame piece values for the specified team
 *
 * @note always returns positive values meaning it still needs to be negated for Team Black
 */
constexpr BoardValue piece_values(const Board& board, Team::Team team, int& mid_game_percentage) {
    BoardValue value = {0};
    for (int i = 1; i < PIECETYPE_AMOUNT; ++i) {
        PieceType::PieceType type = static_cast<PieceType::PieceType>(i);
        int amount_pieces = board.positions.getAmount(team, type);
        value.mg += amount_pieces * mg_value[i];
        value.eg += amount_pieces * eg_value[i];
        mid_game_percentage += amount_pieces * mid_game_factors[type];
    }
    return value;
}

/**
 * @brief Calculates the Positional advantage for a given team.
 *
 * @tparam team the team for which to calculate the positional advantage
 * @param board The current board state
 * @return A BoardValue struct containing the midgame and endgame Positional advantage values
 *
 * @note always returns positive values meaning it still needs to be negated for Team Black
 */
template<Team::Team team>
constexpr BoardValue positional_advantage(const Board& board) {
    constexpr int flip_row = team == Team::WHITE ? 0 : 56;
    BoardValue value = {};
    for (int i = 1; i < PIECETYPE_AMOUNT; ++i) {
        PieceType::PieceType type = static_cast<PieceType::PieceType>(i);
        Board_Positions::PositionsSlice pos_list = board.positions.getVec(team, type);
        for (int j = 0; j < pos_list.size; ++j) {
            int8_t current_pos = pos_list[j];
            int8_t table_pos = current_pos ^ flip_row;
            value.mg += mg_pesto_table[type][table_pos];
            value.eg += eg_pesto_table[type][table_pos];
        }
    }
    return value;
}

/**
 * @brief Calculates how well the king of the given team is protected.
 *
 * @param board The current board state
 * @param team The team for which to calculate the king protection score.
 * @return The king protection score (i.e. how well the king is currently protected).
 *
 * @note always returns positive values meaning it still needs to be negated for Team Black
 */
constexpr int protected_king(const Board& board, Team::Team team) {
    int fields_protected = 0;
    int8_t king_pos = board.positions.getKingPos(team);
    Vec2 king_pos_2d = Vec2::newVec2(king_pos);
    constexpr Vec2 king_offset_list[8] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}, {0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    for (Vec2 offset : king_offset_list) {
        Vec2 new_pos_2D = king_pos_2d + offset;
        if (new_pos_2D.outsideBoard()) {
            fields_protected++;
            continue;
        }
        Piece protecting_piece = board.grid[new_pos_2D.getPos()];
        if (protecting_piece.team == team) {
            if (protecting_piece.type == PieceType::PAWN)
                fields_protected += 2;
            else
                fields_protected++;
        }
    }

    return fields_protected * protected_king_factor;
}

// function inside header due to wanting to make the function constexpr and the function only being called by MinMax
/**
 * @brief Evaluates the current board.
 *
 * @param board The current board state
 * @param player The player who can currently make a move
 * @return The calculated score for a given board
 *
 * @note
 * positive value : white's advantage \n
 * negative value : black's advantage
 */
constexpr int evaluate(const Board& board, Team::Team player) {
    const int current_player_factor = player == Team::WHITE ? 1 : -1;
    int result_score = 0;
    int mid_game_phase_percentage = 0;

    // piece Value
    BoardValue white_piece_value = piece_values(board, Team::WHITE, mid_game_phase_percentage);
    BoardValue black_piece_value = piece_values(board, Team::BLACK, mid_game_phase_percentage);

    // pesto
    BoardValue white_pesto = positional_advantage<Team::WHITE>(board);
    BoardValue black_pesto = positional_advantage<Team::BLACK>(board);

// calculate Pesto current board value with (https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function)
// modified because the original code is basically unintelligible

    // score as if the board is currently in the midgame
    int midgame_score = (white_piece_value.mg - black_piece_value.mg) + (white_pesto.mg - black_pesto.mg);
    // score as if the board is currently in the endgame
    int endgame_score = (white_piece_value.eg - black_piece_value.eg) + (white_pesto.eg - black_pesto.eg);
    // cap midgame phase percentage to 24
    mid_game_phase_percentage = std::min(mid_game_phase_percentage, max_phase_percentage);
    // make inverse of midgame phase percentage into endgame phase percentage
    int endgame_phase_percentage = max_phase_percentage - mid_game_phase_percentage;

    // interpolate between the data depending on how far the current board state is in the endgame
    // removed "/ 24", because if the score can fit in an int why make it smaller again and lose precision
    result_score += (midgame_score * mid_game_phase_percentage) + (endgame_score * endgame_phase_percentage);

//extra

    // king is protected
    int difference_king_protection = protected_king(board, Team::WHITE) - protected_king(board, Team::BLACK);
    // reuse the previously calculated percentage of how far the board is still in the midgame here
    result_score += difference_king_protection * mid_game_phase_percentage;

    // just realized: basically useless because all scores are calculated ath the same depth
    // meaning the following just shifts all values in one direction for now
    // will get useful if evaluate is called at different depths
#ifdef NON_DEBUG
    // mover's advantage
    result_score += current_player_factor * movers_advantage * max_phase_percentage;
#endif
    return result_score;
}