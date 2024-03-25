//
// Created by LapisApple on 3/24/24.
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

// Perft with Undo
inline uint64_t Perft_withUndo_impl(Board& board, int depth, Team::Team team) {
  if (depth <= 0) return 1;

  const Team::Team enemy = Team::getEnemyTeam(team);
  uint64_t amount_boards = 0;

  std::vector<Move> move_list;
  PossibleMoves::getAllPossibleMoves(board, move_list, team);

  for (const Move move : move_list) {
    const ExecutedMove done_move = board.movePiece(move);
    if (!Check::isChecked(board, team, board.positions.getKingPos(team)))
      amount_boards += Perft_withUndo_impl(board, depth - 1, enemy);
    board.undoMove(done_move);
  }
  return amount_boards;
}

inline uint64_t Perft_withUndo(Board& board, int depth, Team::Team team) {
  const Team::Team enemy = Team::getEnemyTeam(team);

  // is it even a legal game state?
  if (!board.positions.hasPiece(Team::WHITE, PieceType::KING) ||
      !board.positions.hasPiece(Team::BLACK, PieceType::KING))
    return 0;
  // has the game already ended?
  if (Check::isChecked(board, enemy, board.positions.getKingPos(enemy))) return 0;
  // after invariance is established call the actual Perft implementation
  const uint64_t perft_res = Perft_withUndo_impl(board, depth, team);
  return perft_res;
}

#define GARBAGE_DATA 3
#define MAX_PERFT 4
#define MIN_PERFT 1

TEST(undoPerftTest, PerftWithUndoCorrectness) {
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
      uint64_t perft = Perft_withUndo(board, i, fen.current_player);
      std::cout << perft;
      ASSERT_EQ(perft, perft_data[i]);
    }
  }
}
