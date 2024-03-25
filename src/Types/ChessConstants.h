//
// Created by timap on 19.03.2024.
//

#pragma once
#include <cstdint>

/**
 * @brief a namespace holding most constants for chess
 */
namespace ChessConstants {
// castling

// white
/**
 * @brief the starting position of the white king
 */
constexpr inline int8_t start_white_king_pos = 60;
/**
 * @brief the starting position of the white queen side castle
 */
constexpr inline int8_t start_white_queen_side_castle_pos = 56;
/**
 * @brief the starting position of the white king side castle
 */
constexpr inline int8_t start_white_king_side_castle_pos = 63;

// black
/**
 * @brief the starting position of the black king
 */
constexpr inline int8_t start_black_king_pos = 4;
/**
 * @brief the starting position of the black queen side castle
 */
constexpr inline int8_t start_black_queen_side_castle_pos = 0;
/**
 * @brief the starting position of the black king side castle
 */
constexpr inline int8_t start_black_king_side_castle_pos = 7;

// en passant
/**
 * @brief  the row where the passant position is located for black
 * @note This row is different from where the piece, that can be taken by passant, is actually located
 */
constexpr inline int8_t black_en_passant_row = 2;
/**
 * @brief  the row where the passant position is located for white
 * @note This row is different from where the piece, that can be taken by passant, is actually located
 */
constexpr inline int8_t white_en_passant_row = 5;
}  // namespace ChessConstants
