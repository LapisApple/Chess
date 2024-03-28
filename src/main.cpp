//
// Created by tim.apel on 1/12/24.
//

#include "MainGameLoop.h"


#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "../Test/Main_Folder_Path_For_Testing.h"

namespace Check {
    bool atomicCheck(const Board& board, Team::Team player) {
        Team::Team enemy = Team::getEnemyTeam(player);

        std::vector<Move> move_list;
        PossibleMoves::getAllPossibleAtomicMoves(board, move_list, enemy);

        for (const Move& move : move_list) {
            Board tempBoard = board;
            tempBoard.movePiece(move);
            const bool lost_player_king = !tempBoard.positions.hasPiece(player, PieceType::KING);
            if (lost_player_king) return true;
        }

        return false;
    }
}

// Perft without Undo
inline uint64_t Perft_impl(Board board, int depth, Team::Team team, Move prev_move) {
    Team::Team enemy = Team::getEnemyTeam(team);
    // get the new board

    board.movePiece(prev_move);
    if (Check::hasLostKing(board)) return 0;
    if (Check::actualAtomicCheck(board, enemy, board.positions.getKingPos(enemy))) return 0;

    if (depth <= 0) return 1;
    uint64_t amount_boards = 0;

    std::vector<Move> move_list;
    PossibleMoves::getAllPossibleAtomicMoves(board, move_list, team);

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
    PossibleMoves::getAllPossibleAtomicMoves(board, move_list, team);

    for (const Move move : move_list) {
        uint64_t amount = Perft_impl(board, depth - 1, enemy, move);
        // std::cout << Print::move_as_string(move, board.isMoveCapture(move)) << " - " << amount << '\n';
        amount_boards += amount;
    }
    return amount_boards;
}

#define GARBAGE_DATA 3
#define MAX_PERFT 1
#define MIN_PERFT 1

void atomicPerft() {
    std::ifstream file(main_folder_path + "/Test/gtest/datasets/atomic_perft_short.txt");
    assert(file.good());
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
            assert(false);
        }

        Board board = Board(fen);

        std::string perft_str;
        std::vector<uint64_t> perft_data = {0};
        while (std::getline(line_stream, perft_str, ';')) {
            int64_t perf_res = std::stoi(perft_str.substr(GARBAGE_DATA));
            perft_data.push_back(perf_res);
        }

        for (uint64_t i = MIN_PERFT; (i < perft_data.size()) && (i <= MAX_PERFT); ++i) {
            uint64_t perft = Perft(board, static_cast<int>(i), fen.current_player);

            std::cout << fen_str << '\n';
            std::cout << perft << " - " << perft_data[i] << "\n\n";
            assert(perft == perft_data[i]);
        }
    }
}


int main() {
    atomicPerft();
    std::cout << "fin";
    // Chess::mainGameLoop();
}
