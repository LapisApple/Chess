//
// Created by timap on 12.03.2024.
//
#include <gtest/gtest.h>

#include "../../src/Types/BasicChessTypes.h"

TEST(Team_enum_values, correct_enum_values) {
  ASSERT_TRUE(Team::WHITE == 0 || Team::WHITE == 1);
  ASSERT_TRUE(Team::BLACK == 0 || Team::BLACK == 1);
  ASSERT_FALSE(Team::WHITE == Team::BLACK);
}

TEST(Piece_Type_values, correct_PieceType_values) {
  ASSERT_EQ(PieceType::NONE, 0);
  ASSERT_EQ(PieceType::PAWN, 6);
}

TEST(Castling_Rights_enum_test, correct_loseCastlingBoth_value) {
  uint8_t castling_right_lose_both = static_cast<uint8_t>(SpecialMove::loseCastlingKingSide) |
                                     static_cast<uint8_t>(SpecialMove::loseCastlingQueenSide);
  ASSERT_EQ(static_cast<uint8_t>(SpecialMove::loseCastlingBoth), castling_right_lose_both);
}

constexpr uint8_t get_amount_of_castling_flags() {
  constexpr int offset_white = Team::WHITE << 1;
  constexpr int offset_black = Team::BLACK << 1;

  constexpr uint8_t loseCastlingBoth = static_cast<uint8_t>(SpecialMove::loseCastlingBoth);

  uint8_t full_castling_rights = loseCastlingBoth << offset_white | loseCastlingBoth << offset_black;

  int sum = 0;
  for (int i = 0; i < 8; i++) {
    sum += full_castling_rights & 1;
    full_castling_rights >>= 1;
  }
  return sum;
}

TEST(Castling_Rights_enum_test, correct_loseCastling_value_and_Team_value_for_castling_right) {
  ASSERT_EQ(get_amount_of_castling_flags(), 4);
}
