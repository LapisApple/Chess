//
// Created by timap on 17.03.2024.
//

#include <fstream>
#include <string>
#include <iosfwd>
#include <sstream>
#include <gtest/gtest.h>

#include "../Implementation/Perft.h"

// strongly "inspire" by https://www.chessprogramming.org/Perft#Pseudo_Legal_Moves
#define GARBAGE_DATA 3
#define MAX_PERFT 4
#define MIN_PERFT 1

uint64_t reaad_int(std::string& string) {
  uint64_t offset = 0;
  while (string[offset] == ' ' && offset < string.size()) offset++;
  if (string[offset] == 'D') offset += 2;
  while (string[offset] == ' ' && offset < string.size()) offset++;

  uint64_t sum = 0;
  for (;offset < string.size(); ++offset) {
    char ch = string[offset];
    if (ch < '0' || ch > '9') break;
    sum = sum * 10 + ch - '0';
  }
  return sum;
}

TEST(PerftTest, Correctness) {
    std::ifstream file(main_folder_path + "/Test/gtest/datasets/perft.txt");
    ASSERT_TRUE(file.good()) << "please change the path of the perf.txt file in gtest/ if you see this error";
    std::string line;
    while(std::getline(file, line)) {
        std::istringstream line_stream(line);
        std::string fen_str;

        std::getline(line_stream, fen_str, ';');

        Fen fen;
        try {
          fen = Fen::buildFenFromStr(fen_str);
        } catch (FenParsingException fenException) {
          std::cerr << fenException.what();
        }

        Board board = Board(fen);

        std::string perft_str;
        std::vector<uint64_t> perft_data = {0};
        while (std::getline(line_stream, perft_str, ';')) {
            int64_t perf_res = std::stoi(perft_str.substr(GARBAGE_DATA));
            perft_data.push_back(perf_res);
        }


        for (int i = MIN_PERFT; (i < perft_data.size()) && (i <= MAX_PERFT); ++i) {
            uint64_t perft = Perft(board, i, fen.currentTurn);
            std::cout << perft;
            ASSERT_EQ(perft, perft_data[i]);
        }

    }


}