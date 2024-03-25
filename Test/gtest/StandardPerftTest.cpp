//
// Created by timap on 17.03.2024.
//

#include <gtest/gtest.h>

#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "../../src/Board/Board.h"
#include "../../src/Check/Check.h"
#include "../../src/PossibleMoves/PossibleMoves.h"
#include "../Main_Folder_Path_For_Testing.h"

// Perft without Undo
inline uint64_t Perft_impl(Board board, int depth, Team::Team team, Move prev_move) {
  Team::Team enemy = Team::getEnemyTeam(team);
  // get the new board

  board.movePiece(prev_move);
  if (Check::isChecked(board, enemy, board.positions.getKingPos(enemy))) return 0;

  if (depth <= 0) return 1;
  uint64_t amount_boards = 0;

  std::vector<Move> move_list;
  PossibleMoves::getAllPossibleMoves(board, move_list, team);

  for (const Move next_move : move_list) {
    amount_boards += Perft_impl(board, depth - 1, enemy, next_move);
  }
  return amount_boards;
}

inline uint64_t Perft(Board board, int depth, Team::Team team) {
  Team::Team enemy = Team::getEnemyTeam(team);
  if (!board.positions.hasPiece(Team::WHITE, PieceType::KING) ||
      !board.positions.hasPiece(Team::BLACK, PieceType::KING))
    return 0;

  if (depth <= 0) return 1;
  uint64_t amount_boards = 0;

  std::vector<Move> move_list;
  PossibleMoves::getAllPossibleMoves(board, move_list, team);

  for (const Move move : move_list) {
    amount_boards += Perft_impl(board, depth - 1, enemy, move);
  }
  return amount_boards;
}

#define GARBAGE_DATA 3
#define MAX_PERFT 4
#define MIN_PERFT 1

TEST(PerftTest, StandardPerftCorrectness) {
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

    std::string perft_str;
    std::vector<uint64_t> perft_data = {0};
    while (std::getline(line_stream, perft_str, ';')) {
      int64_t perf_res = std::stoi(perft_str.substr(GARBAGE_DATA));
      perft_data.push_back(perf_res);
    }

    for (uint64_t i = MIN_PERFT; (i < perft_data.size()) && (i <= MAX_PERFT); ++i) {
      uint64_t perft = Perft(board, i, fen.current_player);
      std::cout << perft;
      ASSERT_EQ(perft, perft_data[i]);
    }
  }
}
