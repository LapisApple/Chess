//
// Created by timap on 24.03.2024.
//
#include <gtest/gtest.h>

#include "../../src/ChessGame.h"

int getEval(const std::string_view& fen_str) {
  Fen fen = Fen();
  try {
    fen = Fen::buildFenFromStr(fen_str);
  } catch (const FenParsingException& fenException) {
    std::cerr << fenException.what() << std::endl;
    std::terminate();
  }
  ChessGame board = ChessGame(fen, Team::BLACK);

  int score = board.evaluate();
  return score;
}

TEST(EvalTest, not_completely_wrong_board_evaluation) {
  ASSERT_EQ(getEval("k7/7B/8/8/8/8/7b/K7 w - - 0 1"), getEval("k7/7B/8/8/8/8/7b/K7 b - - 0 1"));
  ASSERT_EQ(0, getEval("k7/7B/8/8/8/8/7b/K7 w - - 0 1"));
  ASSERT_EQ(0, getEval("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

  // 200 = a small advantage (around 1/12 of an extra pawn)
  // to stop small differences in positioning from returning true even if it is incorrect
  ASSERT_LT(200, getEval("k7/7P/8/8/8/7p/8/K7 w - - 0 1"));
  ASSERT_LT(200, getEval("rnbqkbnr/8/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
}
