//
// Created by timap on 23.03.2024.
//

#pragma once
#include <fstream>
#include <sstream>

#include "../../../src/ChessGame.h"
#include "../Perft.h"

#include "../../Main_Folder_Path_For_Testing.h"

#define GARBAGE_DATA 3
#define MAX_PERFT 4

void perftAssert(int depth, int val, std::string_view str) {
    Fen fen;
    try {
        fen = Fen::buildFenFromStr(str);
    } catch (const FenParsingException& fenException) {
        std::cerr << fenException.what() << std::endl;
        std::terminate();
    }
    Board board = Board(fen);
    uint64_t res = Perft_withUndo(board, depth, fen.current_player);

    if (res == val) {
        std::cout << val << " - " << res << std::endl;
    } else {
        std::cerr << val << " - " << res << " - depth: " << depth << " | " << str  << std::endl;
    }

    assert(res == val);
}

void debugPerftSelectedOnly() {
//    perftAssert(2, 2039, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
//
//    perftAssert(2, 400, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
//    perftAssert(3, 8902, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
//    perftAssert(4, 197281, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
//    perftAssert(5, 4865609, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

//    perftAssert(2, 66, "4k3/8/8/8/8/8/8/4K2R w K - 0 1");
//    perftAssert(2, 71, "4k3/8/8/8/8/8/8/R3K3 w Q - 0 1");
//    perftAssert(2, 75, "4k2r/8/8/8/8/8/8/4K3 w k - 0 1");
//
//    perftAssert(2, 49, "r3k3/1K6/8/8/8/8/8/8 w q - 0 1");

//    perftAssert(1, 11, "8/Pk6/8/8/8/8/6Kp/8 w - - 0 1 ");
//    perftAssert(2, 97, "8/Pk6/8/8/8/8/6Kp/8 w - - 0 1");

//    perftAssert(1, 25, "r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1");
//    perftAssert(2, 547, "r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1");
//    perftAssert(3, 13579, "r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1 ");
//    perftAssert(4, 316214, "r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1 ");

//    perftAssert(3, 8034, "4k1r1/8/8/8/8/8/r7/R3K2R w KQ - 1 2");
//    perftAssert(2, 355, "4k1rR/8/8/8/8/8/r7/R3K3 b Q - 2 2");

// undo
//    perftAssert(1, 12, "8/8/8/8/8/8/6k1/4K2R w K - 0 1");
//    perftAssert(1, 11, "8/Pk6/8/8/8/8/6Kp/8 w - - 0 1");
//    perftAssert(1, 24, "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1");
}

void debugPerftFileAll() {
    std::ifstream file(main_folder_path + "/Test/gtest/datasets/perft.txt");
    assert(file.good());
    std::string line;
    while(std::getline(file, line)) {
        std::istringstream line_stream(line);
        std::string fen_str;

        std::getline(line_stream, fen_str, ';');

        Fen fen;
        try {
            std::cout << fen_str << "\n" << std::endl ;
            fen = Fen::buildFenFromStr(fen_str);
        } catch (const FenParsingException& fenException) {
            std::cerr << fenException.what();
        }

        Board board = Board(fen);

        std::string perft_str;
        std::vector<uint64_t> perft_data = {0};
        while (std::getline(line_stream, perft_str, ';')) {
            int64_t perf_res = std::stoi(perft_str.substr(GARBAGE_DATA));
            perft_data.push_back(perf_res);
        }


        for (int i = 1; (i < perft_data.size()) && (i <= MAX_PERFT); ++i) {
            std::cout << fen_str << "\n" << " depth: " << i <<"perft: " ;
            uint64_t perft = Perft_withUndo(board, i, fen.current_player);
            std::cout << perft << " expected: " << perft_data[i] << "\n\n" << std::flush;
            assert(perft == perft_data[i]);
        }
    }
}