//
// Created by timap on 14.03.2024.
//

#include "Print.h"
#include <iostream>
#include "../../Types/Vec2.h"

/**
 * @brief Converts a board position to chess notation and appends it to a given string
 *
 * @param move_str The string to which to append the board position
 * @param pos The board position to convert to a string (0=..\<64)
 *
 * @note 0 represents the top left corner of the board (a8 in chess notation)
 * and 63 represents the bottom right corner (h1 in chess notation).
 */
static inline void push_field_position_to_string(std::string& move_str, int8_t pos) {
    Vec2 pos_2D = Vec2::newVec2(pos);
    move_str.push_back(static_cast<char>('a' + pos_2D.x));
    move_str.push_back(static_cast<char>('8' - pos_2D.y));
}

namespace Print {

    // (function documentation is provided in the corresponding header)
    std::string position_to_string(int8_t pos) {
        std::string move_str;
        push_field_position_to_string(move_str, pos);
        return move_str;
    }

    // (function documentation is provided in the corresponding header)
    [[nodiscard]] std::string move_as_string(Move move, bool is_capture) {
        std::string move_str;

        move_str.push_back(move.piece.asChar());

        push_field_position_to_string(move_str, move.from);

        if (is_capture || move.specialMove.data == SpecialMove::en_Passant) move_str.push_back('x');

        push_field_position_to_string(move_str, move.to);

        if (move.promote.has_value()) {
            move_str.push_back('=');
            move_str.push_back(Piece(move.piece.team, move.promote.data).asChar());
        }

        return move_str;
    }

    // (function documentation is provided in the corresponding header)
    void print_board_grid_state(const Board_8x8& board_grid, std::ostream& os) {
        int pos = 0;
        os << "    (A)(B)(C)(D)(E)(F)(G)(H)\n";
        for (int rows = 8; rows > 0; --rows) {
            os << "(" << rows << ") ";
            for (int column = 0; column < 8; ++column) {
                os << '[' << board_grid[pos].asChar() << ']';
                pos++;
            }
            os << '\n';
        }
    }

    // (function documentation is provided in the corresponding header)
    std::string board_state_to_fen(const Board_8x8& grid, Board_Extra extra , Team::Team current_team, int current_turn, int current_half_turn) {
        std::string fen;

        int amount_empty_squares_with_current = 0;

        auto take_take_of_pieces = [&fen, &amount_empty_squares_with_current]() {
            if (amount_empty_squares_with_current != 0) {
                fen.push_back(static_cast<char>(amount_empty_squares_with_current + '0'));
                amount_empty_squares_with_current = 0;
            }
        };

        for (int i = 0; i < BOARD_SIZE; ++i) {
            Piece current = grid[i];

            if (current.team == Team::NONE) amount_empty_squares_with_current++;
            else {
                take_take_of_pieces();
                fen.push_back(current.asChar());
            }

            if (i % 8 == 7) {
                take_take_of_pieces();
                fen.push_back('/');
            }
        }
        fen.pop_back();

        fen.push_back(' ');
        fen.push_back(current_team == Team::WHITE ? 'w' : 'b');
        fen.push_back(' ');

        if (!extra.getCastlingRights(ChessConstants::start_black_king_pos) &&
            !extra.getCastlingRights(ChessConstants::start_white_king_pos))
            fen.push_back('-');
        else {
            if (extra.getCastlingRights(ChessConstants::start_white_king_side_castle_pos)) fen.push_back('K');
            if (extra.getCastlingRights(ChessConstants::start_white_queen_side_castle_pos)) fen.push_back('Q');
            if (extra.getCastlingRights(ChessConstants::start_black_king_side_castle_pos)) fen.push_back('k');
            if (extra.getCastlingRights(ChessConstants::start_black_queen_side_castle_pos)) fen.push_back('q');
        }
        fen.push_back(' ');


        if (extra.getPassantPos().has_value()) {
            std::string pos = Print::position_to_string(extra.getPassantPos().data);
            fen.append(pos);
        } else fen.push_back( '-');
        fen.push_back(' ');

        fen += std::to_string(current_half_turn);
        fen.push_back(' ');
        fen += std::to_string(current_turn);

        return fen;
    }
}