//
// Created by LapisApple on 3/24/24.
//

//
// Created by timap on 17.03.2024.
//

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../../src/Board/Board.h"
#include "../../src/Check/Check.h"
#include "../../src/PossibleMoves/PossibleMoves.h"
#include "../Main_Folder_Path_For_Testing.h"

TEST(FenParsingTest, ParsingPerftFenCorrectly) {
  std::ifstream file(main_folder_path + "/Test/gtest/datasets/perft.txt");
  ASSERT_TRUE(file.good()) << missing_main_folder_path_error;
  std::string line;
  while (std::getline(file, line)) {
    std::istringstream line_stream(line);
    std::string fen_str;

    std::getline(line_stream, fen_str, ';');

    Fen fen;
    try {
      fen = Fen::buildFenFromStr(fen_str);
    } catch (const FenParsingException& fenException) {
      std::cerr << fenException.what();
      ASSERT_TRUE(false) << "unable to read a Fen in the dataset";
    }

    Board board = Board(fen);

    // assert that it was parsed correctly
    std::string reparsedFen =
        Print::board_state_to_fen(board.grid, board.extra, fen.current_player, fen.current_turn, fen.amount_half_moves);
    ASSERT_EQ(fen_str, reparsedFen + ' ') << "pre board";
  }
}

TEST(FenParsingTest, FenAndMoveParsingTest) {
  std::ifstream file(main_folder_path + "/Test/gtest/datasets/ParsingTestData.txt");
  ASSERT_TRUE(file.good()) << missing_main_folder_path_error;
  std::string line;
  while (std::getline(file, line)) {
    std::istringstream line_stream(line);

    std::string fen_str;
    std::getline(line_stream, fen_str, ';');
    Fen fen;
    try {
      fen = Fen::buildFenFromStr(fen_str);
    } catch (const FenParsingException& fenException) {
      std::cerr << fenException.what();
      ASSERT_TRUE(false) << "unable to read a Fen in the dataset";
    }
    Board board = Board(fen);

    std::string move_str;
    std::getline(line_stream, move_str, ';');

    std::string fen_str2;
    std::getline(line_stream, fen_str2, ';');
    Fen fen2;
    try {
      fen2 = Fen::buildFenFromStr(fen_str2);
    } catch (const FenParsingException& fenException) {
      std::cerr << fenException.what();
      ASSERT_TRUE(false) << "unable to read a Fen in the dataset";
    }
    Board board2 = Board(fen2);

    // both boards turned back into Strings stay the same
    ASSERT_EQ(fen_str, Print::board_state_to_fen(board.grid, board.extra, fen.current_player, fen.current_turn,
                                                 fen.amount_half_moves))
        << "pre board";
    ASSERT_EQ(fen_str2, Print::board_state_to_fen(board2.grid, board2.extra, fen2.current_player, fen2.current_turn,
                                                  fen2.amount_half_moves))
        << "post board";

    // assert move is possible
    SlimOptional<UserMove> move = convert_string_to_move(move_str);
    ASSERT_TRUE(move.has_value());

    // move exists as legal move
    std::vector<Move> move_list;
    PossibleMoves::getAllPossibleMoves(board, move_list, fen.current_player);
    PossibleMoves::trimMovesPuttingPlayerIntoCheckmate(board, move_list, fen.current_player);

    SlimOptional<Move> sameMove = SlimOptional<Move>::nullopt();
    for (Move& current_move : move_list) {
      if (move.data.equals(current_move, board.isMoveCapture(current_move))) {
        sameMove.data = current_move;
        break;
      }
    }
    ASSERT_TRUE(sameMove.has_value());

    // move produces 2nd Board
    board.movePiece(sameMove.data);
    ASSERT_EQ(fen_str2, Print::board_state_to_fen(board.grid, board.extra, fen2.current_player, fen2.current_turn,
                                                  fen2.amount_half_moves));
  }
}
