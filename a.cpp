//
// Created by timap on 28.11.2023.
//
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

// Pieces.h =================================================

enum Pieces : char {
    EMPTY_SQUARE = ' ',

    WHITE_KING = 'K',
    WHITE_QUEEN = 'Q',
    WHITE_BISHOP = 'B',
    WHITE_PAWN = 'P',
    WHITE_CASTLE = 'R',
    WHITE_KNIGHT = 'N',

    BLACK_KING = 'k',
    BLACK_QUEEN = 'q',
    BLACK_BISHOP = 'b',
    BLACK_PAWN = 'p',
    BLACK_CASTLE = 'r',
    BLACK_KNIGHT = 'n',
};

enum Team : char { BLACK = 0, WHITE = 1 };

// Move.h =================================================

struct Capture {
    bool isTrue = false;
    Pieces type = EMPTY_SQUARE;
};

struct MoveData {
    Pieces type;
    uint8_t from;
    uint8_t to;
    Capture capture;
    std::optional<Pieces> promotesTo = std::nullopt;
};

// Move.cpp =================================================
//
// Created by timap on 19.11.2023.
//
struct Vec2S {
    int x = 0;  // to the right
    int y = 0;  // down
    int steps = 0;
};

struct Vec2 {
    int x = 0;  // to the right
    int y = 0;  // down
};

Vec2 inline getVec(int from, int to) {
    Vec2 direction;
    direction.x = (to & 7) - (from & 7);
    direction.y = (to >> 3) - (from >> 3);
    //((from % 8) + diff) / 8;

    return direction;
}

Vec2S getNormalVec(int from, int to) {
    Vec2 vec = getVec(from, to);
    Vec2S direction;

    int absX = std::abs(vec.x);
    int absY = std::abs(vec.y);

    int steps = std::max(absY, absX);
    direction.steps = steps;
    direction.x = vec.x / steps;
    direction.y = vec.y / steps;

    return direction;
}

Vec2 getVecAbs(int from, int to) {
    Vec2 direction = getVec(from, to);
    direction.x = std::abs(direction.x);
    direction.y = std::abs(direction.y);

    return direction;
}

bool isMoveObstructed(int from, Vec2S direction, const Pieces* board) {
    int offset = (8 * direction.y + direction.x);
    for (int i = 1; i < direction.steps; ++i) {
        int pos = from + offset * i;
        if (board[pos] != EMPTY_SQUARE) return true;
    }
    return false;
}

bool isMoveObstructed(int from, int to, const Pieces* board) {
    Vec2S direction = getNormalVec(from, to);
    return isMoveObstructed(from, direction, board);
}

//============================================================================
namespace MoveValidator::Move {

    void castleMove(std::optional<MoveData>& move, const Pieces* board) {
        Vec2 vec = getVec(move->from, move->to);

        int notLinear = vec.x * vec.y;
        if (notLinear) {
            move = std::nullopt;
            return;
        }

        if (isMoveObstructed(move->from, move->to, board)) {
            move = std::nullopt;
            return;
        }
    }

    void knightMove(std::optional<MoveData>& move, const Pieces*) {
        // check if it goes 1 in one direction and 2 in another
        Vec2 diff = getVecAbs(move->from, move->to);

        int notZero = diff.x * diff.y;
        if ((diff.x + diff.y) != 3 || !notZero) {
            move = std::nullopt;
        }
    }

    void bishopMove(std::optional<MoveData>& move, const Pieces* board) {
        Vec2 absVec = getVecAbs(move->from, move->to);

        if (absVec.x != absVec.y) {
            move = std::nullopt;
            return;
        }

        if (isMoveObstructed(move->from, move->to, board)) {
            move = std::nullopt;
            return;
        }
    }

    void queenMove(std::optional<MoveData>& move, const Pieces* board) {
        Vec2 absVec = getVecAbs(move->from, move->to);
        int notLinear = absVec.x * absVec.y;
        bool diagonal = absVec.x == absVec.y;

        if (notLinear && !diagonal) {
            move = std::nullopt;
            return;
        }

        if (isMoveObstructed(move->from, move->to, board)) {
            move = std::nullopt;
            return;
        }
    }

    void kingMove(std::optional<MoveData>& move, const Pieces*) {
        Vec2 diff = getVecAbs(move->from, move->to);

        if (diff.y > 1 || diff.x > 1) {
            move = std::nullopt;
        }
    }

    void whitePawnMove(std::optional<MoveData>& move, const Pieces* board) {
        Vec2 absVec = getVecAbs(move->from, move->to);

        if (move->capture.isTrue) {
            // check that it captures diagonally
            if (absVec.x != 1 || absVec.y != 1) {
                move = std::nullopt;
                return;
            }

        } else {
            // check if it moves in one line
            if (absVec.x != 0) {
                move = std::nullopt;
                return;
            }

            // check if it doesn't move too much
            if (absVec.y > 2) {
                move = std::nullopt;
                return;
            }

            // check that it can only move 2 at the beginning
            if (absVec.y == 2) {
                int row = move->from / 8;
                if (row != 6) {
                    move = std::nullopt;
                    return;
                } else {
                    // check if a piece is in the way
                    if (isMoveObstructed(move->from, move->to, board)) {
                        move = std::nullopt;
                        return;
                    }
                }
            }
        }
        // check if it goes in the correct direction
        if (move->to > move->from) {
            move = std::nullopt;
        }
    }

    void blackPawnMove(std::optional<MoveData>& move, const Pieces* board) {
        Vec2 absVec = getVecAbs(move->from, move->to);

        if (move->capture.isTrue) {
            // check that it captures diagonally
            if (absVec.x != 1 || absVec.y != 1) {
                move = std::nullopt;
                return;
            }

        } else {
            // check if it moves in one line
            if (absVec.x != 0) {
                move = std::nullopt;
                return;
            }

            // check if it doesn't move too much
            if (absVec.y > 2) {
                move = std::nullopt;
                return;
            }

            // check that it can only move 2 at the beginning
            if (absVec.y == 2) {
                int row = move->from / 8;
                if (row != 1) {
                    move = std::nullopt;
                    return;
                } else {
                    // check if a piece is in the way
                    if (isMoveObstructed(move->from, move->to, board)) {
                        move = std::nullopt;
                        return;
                    }
                }
            }
        }
        // check if it goes in the correct direction
        if (move->to < move->from) {
            move = std::nullopt;
        }
    }

}  // namespace MoveValidator::Move
// MoveValidator.cpp =================================================

//
// Created by timap on 23.11.2023.
//

int moveStrToIntPos(char character, char number) {
    if ((character < 'a' || character > 'h') && (number < '1' || number > '8')) {
        return -1;
    }

    uint8_t row = character - 'a';
    uint8_t column = number - '0';

    int pos = (8 - column) * 8 + row;
    return pos;
}

bool inline isPieceWhite(Pieces piece) { return (piece & 96) == 64; }

bool inline arePiecesTheSameTeam(Pieces piece1, Pieces piece2) { return (piece1 & 96) == (piece2 & 96); }

bool isPiece(char piece) {
    char pieceNames[6] = {'K', 'Q', 'B', 'R', 'N', 'P'};

    char tempPiece = static_cast<char>(piece & 95);
    for (char ch : pieceNames) {
        if (ch == tempPiece) {
            return true;
        }
    }
    return false;
}

//=====================================================================================
namespace MoveValidator {

    std::optional<MoveData> createMoveFromString(const std::string& input) {
        MoveData move{};

        // check Piece
        if (!isPiece(input[0])) return std::nullopt;
        move.type = static_cast<Pieces>(input[0]);

        // check source position
        int pos = moveStrToIntPos(input[1], input[2]);
        if (pos == -1) return std::nullopt;
        move.from = pos;

        // check target position
        if (input[3] == 'x') {
            pos = moveStrToIntPos(input[4], input[5]);
        } else {
            pos = moveStrToIntPos(input[3], input[4]);
        }
        if (pos == -1) return std::nullopt;
        move.to = pos;

        // check capture
        if (input[3] == 'x') {
            move.capture.isTrue = true;
        }

        // check promotion
        if (input[input.size() - 2] == '=') {
            char tempPiece = input[input.size() - 1];
            if (isPiece(tempPiece)) {
                move.promotesTo = static_cast<Pieces>(tempPiece);

                if (move.type == move.promotesTo.value()) return std::nullopt;
            } else {
                return std::nullopt;
            }
        }

        return move;
    }

// automatically checks that it doesn't move to itself, because it would be a capture (which can't be the same team)
    void checkMoveSyntax(std::optional<MoveData>& move, const Pieces* board) {
        if (!move.has_value()) {
            return;
        }

        Pieces sourcePiece = board[move->from];
        Pieces capturePiece = board[move->to];

        // check if correct Piece was specified
        if (sourcePiece != move->type) {
            move = std::nullopt;
            return;
        }

        // checks if it does capturing how it says
        if (move->capture.isTrue) {
            // check capture existing and not being the same type
            if (capturePiece == EMPTY_SQUARE) {
                move = std::nullopt;
                return;
            }
            if (arePiecesTheSameTeam(sourcePiece, capturePiece)) {
                move = std::nullopt;
                return;
            }
            move->capture.type = capturePiece;
        } else {
            if (capturePiece != EMPTY_SQUARE) {
                move = std::nullopt;
                return;
            }
        }

        // checks correct promotion statement
        if (move->promotesTo.has_value()) {
            // check if pieces are same team
            Pieces promotePiece = move->promotesTo.value();
            if (!arePiecesTheSameTeam(sourcePiece, promotePiece)) {
                move = std::nullopt;
                return;
            }

            // check correct row and if source piece even is a pawn, and if it doesn't try to promote to the king
            if (isPieceWhite(sourcePiece)) {
                if (move->to > 7 || sourcePiece != WHITE_PAWN || promotePiece == WHITE_KING) {
                    move = std::nullopt;
                    return;
                }
            } else {
                if (move->to < 56 || sourcePiece != BLACK_PAWN || promotePiece == BLACK_KING) {
                    move = std::nullopt;
                    return;
                }
            }
        }
    }

    void checkMoveTurn(std::optional<MoveData>& move, bool whiteTurn) {
        if (!move.has_value()) {
            return;
        }

        if (whiteTurn != isPieceWhite(move->type)) {
            move = std::nullopt;
        }
    }

    void checkLegalChessMove(std::optional<MoveData>& move, const Pieces* board) {
        if (!move.has_value()) return;

        // check legal chess move
        switch (move.value().type) {
            case WHITE_KING:
            case BLACK_KING:
                Move::kingMove(move, board);
                break;
            case BLACK_QUEEN:
            case WHITE_QUEEN:
                Move::queenMove(move, board);
                break;
            case BLACK_BISHOP:
            case WHITE_BISHOP:
                Move::bishopMove(move, board);
                break;
            case BLACK_CASTLE:
            case WHITE_CASTLE:
                Move::castleMove(move, board);
                break;
            case BLACK_KNIGHT:
            case WHITE_KNIGHT:
                Move::knightMove(move, board);
                break;
            case WHITE_PAWN:
                Move::whitePawnMove(move, board);
                break;
            case BLACK_PAWN:
                Move::blackPawnMove(move, board);
                break;
            case EMPTY_SQUARE:
                move = std::nullopt;
                break;
        }
    }

    std::optional<MoveData> checkMoveAll(const std::string& input, const Pieces* board, bool whiteTurn) {
        std::optional<MoveData> move = createMoveFromString(input);
        checkMoveSyntax(move, board);
        checkMoveTurn(move, whiteTurn);
        checkLegalChessMove(move, board);

        return move;
    }

}  // namespace MoveValidator

// Board.cpp =================================================

namespace Board {

    Pieces* createSetupBoard() {
        /*
        char* test = new char[64] {'r','n','b','q','k','b','n','r',
                                   'p','p','p','p','p','p','p','p',
                                   ' ',' ',' ',' ',' ',' ',' ',' ',
                                   ' ',' ',' ',' ',' ',' ',' ',' ',
                                   ' ',' ',' ',' ',' ',' ',' ',' ',
                                   ' ',' ',' ',' ',' ',' ',' ',' ',
                                   'P','P','P','P','P','P','P','P',
                                   'R','N','B','Q','K','B','N','R'};
        Pieces* board = reinterpret_cast<Pieces *>(test);
        */
        Pieces* board = new Pieces[64];

        board[0] = BLACK_CASTLE;
        board[1] = BLACK_KNIGHT;
        board[2] = BLACK_BISHOP;
        board[3] = BLACK_QUEEN;
        board[4] = BLACK_KING;
        board[5] = BLACK_BISHOP;
        board[6] = BLACK_KNIGHT;
        board[7] = BLACK_CASTLE;

        for (int i = 0; i < 8; ++i) {
            board[i + 8] = BLACK_PAWN;
            board[i + 48] = WHITE_PAWN;
        }

        for (int i = 16; i < 48; ++i) {
            board[i] = EMPTY_SQUARE;
        }

        board[56] = WHITE_CASTLE;
        board[57] = WHITE_KNIGHT;
        board[58] = WHITE_BISHOP;
        board[59] = WHITE_QUEEN;
        board[60] = WHITE_KING;
        board[61] = WHITE_BISHOP;
        board[62] = WHITE_KNIGHT;
        board[63] = WHITE_CASTLE;

        return board;
    }

    void readBoard(std::istream& file, Pieces* board) {
        char Char;
        for (int i = 0; i < 64; ++i) {
            file.get(Char);
            board[i] = static_cast<Pieces>(Char);
        }
        // file.get(Char);
    }

    void printBoard(const Pieces* board, std::ostream& os) {
        int row = 8;
        os << "    (A)(B)(C)(D)(E)(F)(G)(H)";
        for (uint8_t i = 0; i < 64; ++i) {
            if (i % 8 == 0) {
                os << "\n(" << row << ") ";
                row--;
            }
            os << '[' << static_cast<char>(board[i] & 127) << ']';
        }
        os << '\n';
        /*
        os << board[0];
        for (uint8_t i = 1; i < 64; ++i) {
          if ((i % 8 == 0)) {
            os << '\n';
          }
          os << board[i];
        }
        os << '\n';
         */
    }

    void executeMove(Pieces* board, MoveData move) {
        board[move.to] = board[move.from];
        board[move.from] = EMPTY_SQUARE;

        if (move.promotesTo.has_value()) {
            board[move.to] = move.promotesTo.value();
        }
    }

    void undoMove(Pieces* board, MoveData& move) {
        board[move.to] = move.capture.type;
        board[move.from] = move.type;
    }

}  // namespace Board

// Check.cpp =================================================

constexpr inline Pieces getTeamPiece(int8_t team, char Piece) { return static_cast<Pieces>(Piece | (!team * 32)); }

inline int findPiece(Pieces piece, const Pieces* board) {
    for (int i = 0; i < 64; ++i) {
        if (piece == board[i]) return i;
    }
    return -1;
}

bool insideBoard(int pos) { return pos >= 0 && pos < 64; }

bool insideBoard(Vec2 pos) { return !((pos.x | pos.y) & 0xfff8); }

template <Pieces piece1, Pieces piece2>
inline bool isCheckedLoop(Vec2 kingPos, const Pieces* board, const Vec2* offset) {
    for (int i = 0; i < 4; ++i) {
        Vec2 dir = offset[i];

        Vec2 currentField = {kingPos.x + dir.x, kingPos.y + dir.y};

        while (insideBoard(currentField)) {
            char currentPiece = board[currentField.y * 8 + currentField.x];
            currentField = {currentField.x + dir.x, currentField.y + dir.y};

            if (currentPiece == EMPTY_SQUARE) continue;

            if (currentPiece == piece1 || currentPiece == piece2) {
                return true;
            } else {
                break;
            }
        }
    }
    return false;
}

inline Vec2 add(Vec2 lhs, Vec2 rhs) { return {lhs.x + rhs.x, lhs.y + rhs.y}; }

inline int posFromVec(Vec2 vec) { return vec.y * 8 + vec.x; }

template <Team team>
bool isChecked(const Pieces* board) {
    constexpr Pieces enemyKing = getTeamPiece(!team, WHITE_KING);
    constexpr Pieces enemyKnight = getTeamPiece(!team, WHITE_KNIGHT);
    constexpr Pieces enemyPawn = getTeamPiece(!team, WHITE_PAWN);
    constexpr Pieces enemyQueen = getTeamPiece(!team, WHITE_QUEEN);
    constexpr Pieces enemyBishop = getTeamPiece(!team, WHITE_BISHOP);
    constexpr Pieces enemyCastle = getTeamPiece(!team, WHITE_CASTLE);
    constexpr Pieces king = static_cast<Pieces>(enemyKing ^ 32);
    int piecePos = findPiece(king, board);
    Vec2 pieceVec = getVec(0, piecePos);

    // knight test
    constexpr Vec2 knightOffset[8] = {{-2, -1}, {-2, 1}, {2, -1}, {2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}};
    for (Vec2 knightOf : knightOffset) {
        Vec2 knightPos = add(pieceVec, knightOf);
        if (insideBoard(knightPos) && board[posFromVec(knightPos)] == enemyKnight) return true;
    }

    // pawn test
    constexpr int8_t pawnDir = team == WHITE ? -1 : 1;
    constexpr Vec2 pawnOff[2] = {{-1, pawnDir}, {1, pawnDir}};
    for (Vec2 pawns : pawnOff) {
        Vec2 newPos = add(pieceVec, pawns);
        if (insideBoard(newPos)) {
            if (board[posFromVec(newPos)] == enemyPawn) return true;
        }
    }

    // king test
    constexpr Vec2 dirs[8] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}, {0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    for (Vec2 dir : dirs) {
        Vec2 currentField = add(pieceVec, dir);
        if (!insideBoard(currentField)) continue;

        char currentPiece = board[posFromVec(currentField)];
        if (currentPiece == enemyKing) return true;
    }

    // Bishop + 1/2 QUEEN
    constexpr Vec2 diags[4] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
    bool bishopCheck = isCheckedLoop<enemyBishop, enemyQueen>(pieceVec, board, diags);
    if (bishopCheck) return true;

    // CASTLE + 1/2 QUEEN
    constexpr Vec2 lines[4] = {
            {0, -1},
            {0, 1},
            {-1, 0},
            {1, 0},
    };
    bool castleCheck = isCheckedLoop<enemyCastle, enemyQueen>(pieceVec, board, lines);
    if (castleCheck) return true;

    return false;
}

// Game.h =================================================

enum Result { SUCCESS, FAILURE };

struct GameState {
    bool whiteTurn;
    Pieces* board;
    std::vector<MoveData> moves;
};

class Game {

    GameState gameState{};
public:
    // public:
    Game();
    ~Game();

    Result move(const std::string& moveStr);

    Result unmove();

    void print(std::ostream& os) const;

    void readBoard(std::istream& file);

    bool isCheckWhite() const;

    bool isCheckBlack() const;

    bool isWhiteTurn() const {
        return gameState.whiteTurn;
    }

    [[nodiscard]] Pieces* getBoard() const;
};

// Game.cpp =================================================
//
// Created by timap on 24.11.2023.
//
/*
    (A) (B) (C) (D) (E) (F) (G) (H)

(8) [ 0][ 1][ 2][ 3][ 4][ 5][ 6][ 7]
(7) [ 8][ 9][  ][  ][  ][  ][  ][  ]
(6) [16][  ][  ][  ][  ][  ][  ][  ]
(5) [24][  ][  ][  ][  ][  ][  ][  ]
(4) [32][  ][  ][  ][  ][  ][  ][  ]
(3) [40][  ][  ][  ][  ][  ][  ][  ]
(2) [48][  ][  ][  ][  ][  ][  ][  ]
(1) [56][  ][  ][  ][60][  ][  ][63]
*/

Game::Game() {
    Pieces* board = Board::createSetupBoard();
    gameState = GameState{true, board, std::vector<MoveData>()};
}

Game::~Game() { delete gameState.board; }

void Game::print(std::ostream& os) const { Board::printBoard(gameState.board, os); }

Result Game::move(const std::string& moveStr) {
    // generate and check move
    auto move = MoveValidator::checkMoveAll(moveStr, gameState.board, gameState.whiteTurn);
    if (!move.has_value()) return FAILURE;

    // execute move
    Board::executeMove(gameState.board, move.value());
    gameState.moves.push_back(move.value());
    gameState.whiteTurn = !gameState.whiteTurn;
    return SUCCESS;
}

Result Game::unmove() {
    // check that a move can be undone
    unsigned long size = gameState.moves.size();
    if (size < 1) return FAILURE;

    // undo a move
    Board::undoMove(gameState.board, gameState.moves[size - 1]);
    gameState.moves.pop_back();
    gameState.whiteTurn = !gameState.whiteTurn;
    return SUCCESS;
}

void Game::readBoard(std::istream& file) {
    Board::readBoard(file, gameState.board);
    gameState.whiteTurn = true;
}

bool Game::isCheckWhite() const {
    bool out = isChecked<WHITE>(gameState.board);
    return out;
}

bool Game::isCheckBlack() const {
    bool out = isChecked<BLACK>(gameState.board);
    return out;
}

Pieces* Game::getBoard() const { return this->gameState.board; }

// possible moves: Pieces =================================================

struct PieceAndPos {
    Pieces type;
    Vec2 pos;
};

// #define maxAmountOfMovesEver 32
#define checkMateMove (-1)

struct MoveBuffer {
    int size;
    int moves[32];
};

struct PossibleMoveBoard {
    char board[64];
};

void getPossibleMoveOffset(PieceAndPos piece, MoveBuffer& buffer, const Pieces* board, const Vec2* offsets) {
    for (int j = 0; j < 8; ++j) {
        Vec2 offset = offsets[j];
        Vec2 newPos = add(piece.pos, offset);

        if (!insideBoard(newPos)) continue;
        int pos = posFromVec(newPos);
        if (arePiecesTheSameTeam(board[pos], piece.type)) continue;

        buffer.moves[buffer.size] = pos;
        buffer.size++;
    }
}

template <Team team>
void getPossiblePawnMove(PieceAndPos piece, MoveBuffer& buffer, const Pieces* board) {
    constexpr int startLine = team == WHITE ? 6 : 1;
    constexpr int8_t pawnDir = team == WHITE ? -1 : 1;
    constexpr Vec2 captureOffset[2] = {{-1, pawnDir}, {1, pawnDir}};
    constexpr Vec2 pawnMovement[2] = {{0, pawnDir}, {0, pawnDir * 2}};

    const bool isStartLine = piece.pos.y == startLine;

    // normal moves
    for (int i = 0; i < 1 + isStartLine; ++i) {
        Vec2 offset = pawnMovement[i];
        Vec2 newPos = add(piece.pos, offset);

        if (!insideBoard(newPos)) continue;
        int pos = posFromVec(newPos);
        if (board[pos] != EMPTY_SQUARE) break;

        buffer.moves[buffer.size] = pos;
        buffer.size++;
    }

    // possible Capture Moves
    for (Vec2 pawns : captureOffset) {
        Vec2 newPos = add(piece.pos, pawns);

        if (!insideBoard(newPos)) continue;
        int pos = posFromVec(newPos);
        Pieces capturePiece = board[pos];
        if (arePiecesTheSameTeam(capturePiece, piece.type) || capturePiece == EMPTY_SQUARE) continue;

        buffer.moves[buffer.size] = pos;
        buffer.size++;
    }
}

void getPossibleKingMove(PieceAndPos piece, MoveBuffer& buffer, const Pieces* board) {
    constexpr Vec2 dirs[8] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}, {0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    getPossibleMoveOffset(piece, buffer, board, dirs);
}

void getPossibleKnightMove(PieceAndPos piece, MoveBuffer& buffer, const Pieces* board) {
    constexpr Vec2 knightOffset[8] = {{-2, -1}, {-2, 1}, {2, -1}, {2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}};

    getPossibleMoveOffset(piece, buffer, board, knightOffset);
}

void getPossibleMoveLinearSearch(PieceAndPos piece, MoveBuffer& buffer, const Pieces* board, const Vec2* lines) {
    for (int j = 0; j < 4; ++j) {
        Vec2 line = lines[j];

        Vec2 newPos = add(piece.pos, line);
        while (insideBoard(newPos)) {
            int pos = posFromVec(newPos);
            Pieces currentPiece = board[pos];

            bool sameTeam = arePiecesTheSameTeam(currentPiece, piece.type);
            if (sameTeam) break;

            buffer.moves[buffer.size] = pos;
            buffer.size++;

            if (currentPiece != EMPTY_SQUARE) {
                break;
            }

            newPos = add(newPos, line);
        }
    }
}

void getPossibleCastleMove(PieceAndPos piece, MoveBuffer& buffer, const Pieces* board) {
    constexpr Vec2 lines[4] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    getPossibleMoveLinearSearch(piece, buffer, board, lines);
}

void getPossibleBishopMove(PieceAndPos piece, MoveBuffer& buffer, const Pieces* board) {
    constexpr Vec2 diags[4] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
    getPossibleMoveLinearSearch(piece, buffer, board, diags);
}

void getPossibleQueenMove(PieceAndPos piece, MoveBuffer& buffer, const Pieces* board) {
    getPossibleCastleMove(piece, buffer, board);
    getPossibleBishopMove(piece, buffer, board);
}

void getPossibleMoves(PieceAndPos piece, MoveBuffer& buffer, const Pieces* board) {
    switch (piece.type) {
        case WHITE_KING:
        case BLACK_KING:
            getPossibleKingMove(piece, buffer, board);
            break;
        case WHITE_QUEEN:
        case BLACK_QUEEN:
            getPossibleQueenMove(piece, buffer, board);
            break;
        case WHITE_BISHOP:
        case BLACK_BISHOP:
            getPossibleBishopMove(piece, buffer, board);
            break;
        case WHITE_CASTLE:
        case BLACK_CASTLE:
            getPossibleCastleMove(piece, buffer, board);
            break;
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            getPossibleKnightMove(piece, buffer, board);
            break;
        case WHITE_PAWN:
            getPossiblePawnMove<WHITE>(piece, buffer, board);
            break;
        case BLACK_PAWN:
            getPossiblePawnMove<BLACK>(piece, buffer, board);
            break;
        default:
            break;
    }
}
// possible moves ======================================================

void trimCheckMateMoves(PieceAndPos piece, MoveBuffer& buffer, const Pieces* board) {
    const bool baseIsKing = (tolower(piece.type) == BLACK_KING);
    const bool isWhite = isPieceWhite(piece.type);
    const int basePos = posFromVec(piece.pos);

    Pieces tempBoard[64] = {};
    std::memcpy(tempBoard, board, 64);

    for (int i = buffer.size - 1; i >= 0; --i) {
        int newPos = buffer.moves[i];
        Pieces takenPiece = board[newPos];

        // do move
        tempBoard[newPos] = piece.type;
        tempBoard[basePos] = EMPTY_SQUARE;

        // do the check
        const bool kingTakesKing = (BLACK_KING == tolower(takenPiece)) && baseIsKing;
        const bool isCheckmate = isWhite ? isChecked<WHITE>(tempBoard) : isChecked<BLACK>(tempBoard);
        if (!kingTakesKing && isCheckmate) {
            buffer.moves[i] = checkMateMove;
            buffer.size -= (i == buffer.size - 1);
        }

        // undo move
        tempBoard[newPos] = takenPiece;
        tempBoard[basePos] = piece.type;
    }
}

inline void copyBoardToPrint(char* possibleMoveBoard, Pieces* board) { std::memcpy(possibleMoveBoard, board, 64); }

void writeMovesIntoBoard(const MoveBuffer& buffer, PossibleMoveBoard& possibleMoves) {
    for (int i = 0; i < buffer.size; ++i) {
        int pos = buffer.moves[i];
        if (pos == checkMateMove) continue;
        possibleMoves.board[pos] = possibleMoves.board[pos] == EMPTY_SQUARE ? 'o' : 'x';
    }
}

void printPossible(PossibleMoveBoard& possibleMoves, std::ostream& os) {
    auto tempBoard = reinterpret_cast<Pieces*>(possibleMoves.board);
    Board::printBoard(tempBoard, os);
}

void createPossibleMoves(PieceAndPos piece, const Pieces* board) {
    auto betterBoard = const_cast<Pieces*>(board);
    MoveBuffer buffer{};
    PossibleMoveBoard possibleBoard{};
    copyBoardToPrint(reinterpret_cast<char*>(&possibleBoard.board), betterBoard);

    getPossibleMoves(piece, buffer, board);
    trimCheckMateMoves(piece, buffer, board);

    writeMovesIntoBoard(buffer, possibleBoard);
    printPossible(possibleBoard, std::cout);
}

// === checkmate =======================================
template <Team team>
bool isCheckMate(Pieces* board) {
    constexpr Pieces enemyKing = getTeamPiece(!team, WHITE_KING);

    for (int i = 0; i < 64; ++i) {
        auto currentPiece = board[i];
        if (arePiecesTheSameTeam(currentPiece, enemyKing)) continue;
        PieceAndPos pieceAndPos{currentPiece, getVec(0, i)};

        MoveBuffer buffer{};
        getPossibleMoves(pieceAndPos, buffer, board);
        trimCheckMateMoves(pieceAndPos, buffer, board);

        if (buffer.size != 0) return false;
    }
    return true;
}

std::optional<MoveData> tryMoveAndCheckMate(PieceAndPos piece, Pieces* board, const MoveBuffer& buffer) {
    Pieces tempBoard[64] = {};
    std::memcpy(tempBoard, board, 64);

    MoveData currentMove{piece.type, static_cast<uint8_t>(posFromVec(piece.pos)), 0, {false, EMPTY_SQUARE}, std::nullopt};

    if (piece.type == WHITE_PAWN) {
        for (int i = buffer.size - 1; i >= 0; --i) {
            currentMove.to = buffer.moves[i];
            Pieces takenPiece = board[currentMove.to];
            currentMove.capture.type = takenPiece;
            currentMove.capture.isTrue = takenPiece != EMPTY_SQUARE;

            if (getVec(0, currentMove.to).y == 0) {
                constexpr Pieces test[] = {WHITE_QUEEN, WHITE_CASTLE, WHITE_KNIGHT, WHITE_BISHOP};
                for (auto a : test) {
                    currentMove.promotesTo = a;

                    Board::executeMove(tempBoard, currentMove);

                    // do the checkMate
                    bool isCheckmate = isCheckMate<BLACK>(tempBoard);
                    if (isCheckmate) return currentMove;

                    Board::undoMove(tempBoard, currentMove);
                }
            } else {
                Board::executeMove(tempBoard, currentMove);

                // do the checkMate
                bool isCheckmate = isCheckMate<BLACK>(tempBoard);
                if (isCheckmate) return currentMove;

                Board::undoMove(tempBoard, currentMove);
            }
        }
    } else {
        for (int i = buffer.size - 1; i >= 0; --i) {
            currentMove.to = buffer.moves[i];
            Pieces takenPiece = board[currentMove.to];
            currentMove.capture.type = takenPiece;
            currentMove.capture.isTrue = takenPiece != EMPTY_SQUARE;

            Board::executeMove(tempBoard, currentMove);

            // do the checkMate
            bool isCheckmate = isCheckMate<BLACK>(tempBoard);
            if (isCheckmate) return currentMove;

            Board::undoMove(tempBoard, currentMove);
        }
    }
    return std::nullopt;
}

std::optional<MoveData> tryCheckMate(Pieces* board) {
    for (int i = 0; i < 64; ++i) {
        auto currentPiece = board[i];
        if (arePiecesTheSameTeam(currentPiece, BLACK_KING)) continue;
        if (currentPiece == EMPTY_SQUARE) continue;

        PieceAndPos pieceAndPos{currentPiece, getVec(0, i)};
        MoveBuffer buffer{};
        getPossibleMoves(pieceAndPos, buffer, board);

        auto move = tryMoveAndCheckMate(pieceAndPos, board, buffer);
        if (move.has_value()) {
            return move;
        }
    }
    return std::nullopt;
}

std::string makeMove(const MoveData& move) {
    std::string result = "";
    result.push_back(move.type);
    Vec2 from = getVec(0, move.from);
    result.push_back('a' + from.x);
    result.push_back('8' - from.y);

    if (move.capture.isTrue) {
        result.push_back('x');
    }

    Vec2 to = getVec(0, move.to);
    result.push_back('a' + to.x);
    result.push_back('8' - to.y);

    if (move.promotesTo.has_value()) {
        result.push_back('=');
        result.push_back(move.promotesTo.value());
    }

    return result;
}

// main.cpp =================================================

void trashCurrentLine(std::istream& file) {
    char Char;
    while (file.get(Char)) {
        if (Char == '\n') {
            return;
        }
    }
}

inline void print(const std::string& in) { std::cout << in << '\n'; }

void readBoard(std::istream& file, Game& game) {
    game.readBoard(file);

    std::optional<MoveData> move = tryCheckMate(game.getBoard());
    if (!move.has_value()) {
        std::cout << '\n';
    } else {
        std::cout << makeMove(move.value()) << '\n';
    }

    file.get();
    // trashCurrentLine(file);
}

void makeMove(std::istream& file, Game& game) {
    std::string input;

    std::getline(file, input);
    Result res = game.move(input);
    if (res == FAILURE) {
        print("invalid");
    } else {
        bool checkMate = game.isWhiteTurn() ? game.isCheckWhite() : game.isCheckBlack();
        if (checkMate) {
            checkMate = game.isWhiteTurn() ? isCheckMate<WHITE>(game.getBoard()) : isCheckMate<BLACK>(game.getBoard());
        }
        if (checkMate) {
            print("yes");
        } else {
            print("no");
        }
    }
}

PieceAndPos createPieceAndPosFromFile(std::istream& file) {
    PieceAndPos info{};
    char first = 0;
    file.get(first);
    if (!isPiece(first)) return {EMPTY_SQUARE, 0};
    info.type = static_cast<Pieces>(first);

    char second = 0;
    file.get(first);
    file.get(second);

    int result = moveStrToIntPos(first, second);
    if (result == -1) return {EMPTY_SQUARE, 0};

    info.pos = getVec(0, result);

    file.get();
    return info;
}

void makePossibleMoves(std::istream& file, Game& game) {
    PieceAndPos info = createPieceAndPosFromFile(file);
    if (info.type == EMPTY_SQUARE) return;

    createPossibleMoves(info, game.getBoard());
}

// #define TEST

int main(int argc, char* argv[]) {
#ifndef TEST
    if (argc < 2) return -1;
#endif
    Game game;
    char inChar;

#ifdef TEST
    while (std::cin.get(inChar)) {
    switch (inChar) {
      case 'B':
        readBoard(std::cin, game);
        game.print(std::cout);
        break;
        /*
      case 'M':
        makeMove(std::cin, game);
        break;

      case 'F':
        makePossibleMoves(file, game);
        break;
        */
      default:
        trashCurrentLine(std::cin);
        break;
    }
  }
#else
    std::ifstream file(argv[1]);

    while (file.get(inChar)) {
        switch (inChar) {
            case 'B':
                readBoard(file, game);
                break;
                /*
              case 'M':
                makeMove(file, game);
                break;

              case 'F':
                makePossibleMoves(file, game);
                break;
                */
            default:
                trashCurrentLine(file);
                break;
        }
    }
#endif
}