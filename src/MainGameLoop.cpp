//
// Created by timap on 25.03.2024.
//
#include "MainGameLoop.h"

#include <string>

/**
 * @brief Checks if a string starts with a specific substring.
 *
 * @param string The string to check
 * @param starts_with The substring to check if the string starts with.
 * @return true if the string starts with the specified substring, false otherwise.
 */
inline bool stringStartsWith(const std::string_view& string, const std::string_view& starts_with) {
  const uint64_t pos = string.rfind(starts_with, 0);
  return pos == 0;
}

/**
 * @brief Handles the event of a player winning the game by exiting the programm.
 *
 * @param game The current game state.
 */
inline void handlePlayerGameWin(const ChessGame& game) {
  if (game.isCurrentMoveMakerCheckmated()) {
    std::cout << "The Player has won the Chess Game and beaten the Bot" << std::endl;
    std::exit(0);
  }
}

/**
 * @brief Handles the event of a player losing the game by exiting the programm.
 *
 * @param game The current game state.
 */
inline void handlePlayerGameLoss(const ChessGame& game) {
  if (game.isCurrentMoveMakerCheckmated()) {
    std::cout << "The Player has lost the Chess Game and been beaten by the Bot" << std::endl;
    std::exit(0);
  }
}

/**
 * @brief Handles the Player Move and it's consequences (i.e. checkmate)
 *
 * @param game The current game state.
 * @param move_str the current move
 *
 * @return Result::SUCCESS if the move was legal, Result::FAILURE otherwise
 */
[[nodiscard]] Result handlePlayerMove(ChessGame& game, const std::string_view& move_str) {
  // make Player move
  SlimOptional<UserMove> move = convert_string_to_move(move_str);
  // handle input not being a move
  if (!move.has_value()) {
    std::cout << "that was not a move \n";
    return Result::FAILURE;
  }

  // try executing the chess move, not tested for legality.
  Result res = game.executePlayerMove(move.data);
  if (res == Result::FAILURE) {
    std::cout << "move was illegal \n";
    return Result::FAILURE;
  }
  // print the new board state after player move
  game.print_current_board(std::cout);
  // handle whether the player has won with the previous move
  handlePlayerGameWin(game);

  return Result::SUCCESS;
}

/**
 * @brief Handles the Bot's Move and it's consequences (i.e. checkmate)
 *
 * @param game The current game state.
 */
void handleBotMove(ChessGame& game) {
  // let bot make move
  Result res = game.let_bot_make_move();

  // if the Bot was unable to make a move, that should mean that all moves result in his checkmate
  // meaning the player has won
  if (res == Result::FAILURE) {
    std::cout << "Bot was unable to find a move not putting itself in checkmate\n";
    std::cout << "The Player has won the Chess Game and beaten the Bot" << std::endl;
    std::exit(0);
  }
  // print board
  game.print_current_board(std::cout);
  // check whether the bot has put the player in checkmate with his move
  // i.e. handle a possible bot win
  handlePlayerGameLoss(game);
}

/**
 * @brief Handles Parsing a new Board and the following possible checkmate and possible bot move
 *
 * @note after this function it will be the player's turn either way
 *
 * @param game The current game state.
 * @param line the line with the Fen String
 */
void handleFenParsingForChessGame(ChessGame& game, const std::string_view& line) {
  try {
    // get Fen
    std::string_view fen_str = line.substr(8);
    Fen fen = Fen::buildFenFromStr(fen_str);

    // get Bot color
    std::cout << "Which Color should the Bot be playing as? [w/b]" << std::endl;
    std::string bot_color_str;
    std::getline(std::cin, bot_color_str);

    // if there are spaces before the color skip to the color
    uint64_t i = 0;
    for (; (i < bot_color_str.size()) && (bot_color_str[i] == ' '); ++i) {
    }

    // check that a non-space character existed
    if (i >= bot_color_str.size()) {
      std::cerr << "Missing Team for the bot. \n";
      std::cerr << R"(Please specify the team by either entering 'b' or 'w' after re-entering the Fen)" << std::endl;
      return;
    }

    // get color
    Team::Team bot_team;
    switch (bot_color_str[i]) {
      case 'w':
      case 'W':
        bot_team = Team::WHITE;
        break;
      case 'B':
      case 'b':
        bot_team = Team::BLACK;
        break;
      default:
        std::cerr << '\'' << bot_color_str[i] << "\' is not a Team in Chess\n";
        std::cerr << R"(Please specify the team by either entering 'b' or 'w' after re-entering the Fen)" << std::endl;
        return;
    }

    // read into game
    game = ChessGame(fen, bot_team);
  } catch (const FenParsingException& e) {
    std::cerr << e.what() << std::endl;
    return;
  }
  // should only be executed after the board was successfully read

  // print new game state
  game.print_current_board(std::cout);

  // if it is player's move handle player already being checkmated
  if (game.isPlayerTurn()) {
    handlePlayerGameLoss(game);
  } else {
    // otherwise if it's the bots turn
    // handle bot being checkmated
    handlePlayerGameWin(game);
    // and the bot making a move
    handleBotMove(game);
  }
}

/**
 * @brief Handles All Possible Commands
 *
 * @param game The current game state.
 * @param line the line with the given command
 */
void takeCareOfCommands(ChessGame& game, const std::string_view& line) {
  if (stringStartsWith(line, "/fromFen")) {
    handleFenParsingForChessGame(game, line);
  } else if (stringStartsWith(line, "/toFen")) {
    std::cout << game.asFen() << std::endl;
  } else if (stringStartsWith(line, "/allMoves")) {
    game.print_all_possible_moves(std::cout, 5);
  } else if (stringStartsWith(line, "/bestMove")) {
    SlimOptional<Move> best_move = game.getBestMove();
    game.printMove(std::cout, best_move.data);
  } else if (stringStartsWith(line, "/evaluate")) {
    const int score = game.evaluate();
    std::cout << score << std::endl;
  } else if (stringStartsWith(line, "/undo")) {
    // check whether both moves can be undone correctly
    if (game.amountOfUndoableMoves() < 2) {
      std::cerr << "There are not enough moves to undo" << std::endl;
      return;
    }
    // twice because the bot move AND player move need to be undone
    game.undo_move();
    game.undo_move();
    game.print_current_board(std::cout);
  } else {
    std::cout << "unknown command" << std::endl;
  }
}

/**
 * @brief The Main Game Loop of this Chess Engine that does not return at all
 */
[[noreturn]] void Chess::mainGameLoop() {
  // init board and print it
  ChessGame game = ChessGame();
  game.print_current_board(std::cout);

  while (true) {
    // read next line
    std::string line;
    std::getline(std::cin, line);

    // take care of commands if the current line is a command
    if (line[0] == '/') {
      takeCareOfCommands(game, line);
      continue;
    }

    // make Player move and handle it's possible consequences
    Result res = handlePlayerMove(game, line);
    // if the move was not legal read next input and try again
    if (res == Result::FAILURE) continue;

    // handle bot move and it's possible consequences
    handleBotMove(game);
  }
}
