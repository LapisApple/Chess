//
// Created by LapisApple on 1/12/24.
//
#include <fstream>
#include <sstream>

#include "ChessGame.h"

#define PERFT

bool stringStartsWith(const std::string_view& string, const std::string_view& starts_with) {
    const uint64_t pos = string.rfind(starts_with, 0);
    return pos == 0;
}

void takeCareOfCommands(ChessGame& game, const std::string_view& line) {
    if (stringStartsWith(line, "/fromFen")) {
        try {
            std::string_view fen_str = line.substr(8);
            Fen fen = Fen::buildFenFromStr(fen_str);
            game = ChessGame(fen);
        } catch (const FenParsingException& e) {
            std::cerr << e.what() << std::endl;
        }
        game.print_current_board(std::cout);
    } else if (stringStartsWith(line, "/toFen")) {
        std::cout << game.asFen() << std::endl;
    } else if (stringStartsWith(line, "/allMoves")) {
        game.print_all_possible_moves(std::cout, 5);
    }  else if (stringStartsWith(line, "/bestMove")) {
        SlimOptional<Move> best_move = game.getBestMove();
        game.printMove(std::cout, best_move.data);
    } else {
        std::cout << "unknown command" << std::endl;
    }
}

void mainGameLoop() {
    ChessGame game = ChessGame();
    game.print_current_board(std::cout);
    while (true) {
        // check for possible checkMate
        if (game.player_is_in_checkmate()) {
            std::cout << "You lost\n";
            return;
        } else if (game.enemy_is_in_checkmate()) {
            std::cout << "You Won\n";
            return;
        }

        // read next line
        std::string line;
        std::getline(std::cin, line);

        // take care of commands
        if (line[0] == '/') {
            takeCareOfCommands(game, line);
            continue;
        }

        // make move
        SlimOptional<UserMove> move = convert_string_to_move(line);
        if (!move.has_value()) {
            std::cout << "that was not a move \n";
            continue;
        }
        Result res = game.make_move(move.data);
        if (res == Result::FAILURE) {
            std::cout << "move was illegal \n";
            continue;
        }

        // let bot make move
        res = game.let_bot_make_move();
        if (res == Result::FAILURE) {
            std::cout << "Bot was unable to make a move \n";
            continue;
        }
        // print board
        game.print_current_board(std::cout);
    }
}

int main() {
    mainGameLoop();
}

#ifdef A

int main(int argc, char** argv) {

    std::string fen_str;
    std::getline(std::cin, fen_str);

    ChessGame game = ChessGame(fen_str);
    game.print_current_board(std::cout);
    while (true) {
        Result res = Result::SUCCESS;
        std::string input;
        std::cin >> input;

        // convert input
        bool is_capture_move = false;
        SlimOptional<Move> move = convert_string_to_move(input, is_capture_move);
        if (!move.has_value()) {
            std::cout << "move was not possible";
            continue;
        }
        res = game.make_move(move.data);
        if (res == Result::FAILURE) {
            std::cout << "move was not possible";
            continue;
        }
        game.print_current_board(std::cout);


        res = game.let_bot_make_move();
        if (res == Result::FAILURE) {
            std::cout << "bot was unable to make a move";
            continue;
        }
        game.print_current_board(std::cout);
    }
}







int main(int argc, char** argv) {

#ifdef PERFT
    //check_perft();
    //test_perft();
    std::cout << "fin\n";
#endif
    std::string fen_str;
    ChessGame game;
#ifdef TEST
    while (true) {
#endif
        std::getline(std::cin, fen_str);

        game = ChessGame(fen_str);
        game.print_current_board(std::cout);
#ifdef TEST
    }
#endif

    while (true) {
        Result res = Result::SUCCESS;
        std::string input;
        std::cin >> input;

        // convert input
        SlimOptional<UserMove> move = convert_string_to_move(input);
        if (!move.has_value()) {
            std::cout << "move was not possible";
            continue;
        }
        res = game.make_move(move.data);
        if (res == Result::FAILURE) {
            std::cout << "move was not possible";
            continue;
        }
        game.print_current_board(std::cout);

/*
        res = game.let_bot_make_move();
        if (res == Result::FAILURE) {
            std::cout << "bot was unable to make a move";
            continue;
        }
        game.print_current_board(std::cout);
        */
        game.print_all_possible_moves(std::cout);
        std::cin >> input;

        // convert input
        move = convert_string_to_move(input);
        if (!move.has_value()) {
            std::cout << "move was not possible";
            continue;
        }
        res = game.make_move(move.data);
        if (res == Result::FAILURE) {
            std::cout << "move was not possible";
            continue;
        }
        game.print_current_board(std::cout);

    }
}

#endif