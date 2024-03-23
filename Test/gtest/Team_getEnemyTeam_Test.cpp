//
// Created by timap on 12.03.2024.
//
#include <gtest/gtest.h>
#include "../../src/Types/BasicChessTypes.h"

TEST(Team_getEnemyTeam, correct_output_on_defined_input) {
    ASSERT_EQ(Team::getEnemyTeam(Team::WHITE), Team::BLACK) << "argument : Team::WHITE does not produce Team::BLACK";
    ASSERT_EQ(Team::getEnemyTeam(Team::BLACK), Team::WHITE) << "argument : Team::BLACK does not produce Team::WHITE";
}

TEST(Team_getEnemyTeam, asset_catch_on_undefined_input) {
  ASSERT_DEBUG_DEATH(Team::getEnemyTeam(Team::NONE), "");
}

TEST(Piece_Struct, equality_correctness) {
    ASSERT_TRUE(Piece(Team::WHITE, PieceType::PAWN) == Piece(Team::WHITE, PieceType::PAWN));

    ASSERT_FALSE(Piece(Team::WHITE, PieceType::KING) == Piece(Team::WHITE, PieceType::PAWN));
    ASSERT_FALSE(Piece(Team::BLACK, PieceType::PAWN) == Piece(Team::WHITE, PieceType::PAWN));

    ASSERT_FALSE(Piece(Team::BLACK, PieceType::NONE) == Piece(Team::WHITE, PieceType::PAWN));

    // just test all possible combinations that should produce false
    for (int teams = 0; teams < 3; ++teams) {
        for (int other_teams = teams; other_teams < 3; ++other_teams) {
            for (int i = 0; i < PIECETYPE_AMOUNT; ++i) {
                for (int j = i + 1; j < PIECETYPE_AMOUNT; ++j) {
                    ASSERT_FALSE(Piece(static_cast<Team::Team>(teams), static_cast<PieceType::PieceType>(i)) == Piece(static_cast<Team::Team>(other_teams), static_cast<PieceType::PieceType>(j)));
                }
            }
        }
    }
    // just test all possible combinations that should produce true
    for (int teams = 0; teams < 3; ++teams) {
        for (int i = 0; i < PIECETYPE_AMOUNT; ++i) {
            ASSERT_TRUE(Piece(static_cast<Team::Team>(teams), static_cast<PieceType::PieceType>(i)) == Piece(static_cast<Team::Team>(teams), static_cast<PieceType::PieceType>(i)));
        }
    }
}