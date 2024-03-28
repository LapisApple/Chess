//
// Created by timap on 27.01.2024.
//

#include "PossibleMoves.h"

// helper methods
/**
 * @brief Checks if a move is possible (i.e. inside the board and not taking sown piece)
 * for a given piece on a given board by using an offset from the starting position
 *
 * @param board The current state of the chess board
 * @param piece The piece for which to check the move
 * @param from The current position of the piece on the board
 * @param offset The offset to add to the current position of the piece (i.e. the 2D vector from the starting position
 to the end position of the move)

 * @return if the move is possible the end position of the move, otherwise a nullopt
 */
constexpr static inline ChessPos check_if_possible_move(const Board& board, Piece piece, Vec2 from, Vec2 offset) {
  const Vec2 to2D = from + offset;

  if (to2D.outsideBoard()) return ChessPos::nullopt();

  const int8_t to = to2D.getPos();
  if (board.grid[to].team == piece.team) return ChessPos::nullopt();

  return SlimOptional(to);
}

/**
 * @brief Generates all possible moves for a sliding piece in a certain direction on a given board
 *
 * @tparam fn A function that takes an integer representing a position on the board and returns void.
 * This function will be called for each possible move generated by this function.
 *
 * @param board The current state of the chess board
 * @param forEachPossible The function to call for each possible move
 * @param sliding_piece The sliding piece for which to add all possible moves in a certain direction
 * @param from The current position of the piece on the board
 * @param step the offset/direction to "slide through" until the piece gets stopped
 *
 * This function generates all possible moves for a sliding piece (rook, bishop, queen) in a certain direction on a
 * given board. It does this by starting from the current position of the piece and moving in the direction specified by
 * the step parameter, until it reaches the edge of the board or encounters another piece.
 */
template <typename fn>
constexpr static void get_possible_move_linear_search(const Board& board, fn forEachPossible, Piece sliding_piece,
                                                      Vec2 from, Vec2 step) {
  for (Vec2 to2D = from + step; !to2D.outsideBoard(); to2D += step) {
    const int8_t to = to2D.getPos();
    const Piece currentPiece = board.grid[to];

    if (currentPiece.team == sliding_piece.team) break;
    forEachPossible(to);
    if (currentPiece.type != PieceType::NONE) break;
  }
}

// king castling checks
/**
 * @brief Checks whether the path between king and castle is free for castling
 *
 * @param board the current chess board state
 * @param king_pos the current position of the king
 * @param castle_pos the current position of the castle to castle with
 * @param step the direction from the king to the castle (i.e. 1 for king side and -1 for queen side)

 * @return True if the path is free, false otherwise
 */
constexpr static bool path_is_free_between_king_and_castle(const Board& board, int king_pos, int castle_pos,
                                                           int8_t step) {
  for (int current_pos = king_pos + step; current_pos != castle_pos; current_pos += step) {
    if (board.grid[current_pos].team != Team::NONE) return false;
  }

  return true;
}

/**
 * @brief Checks whether the king is in check or would be passing through check or end up in Check while castling
 *
 * @param board the current chess board state
 * @param king the king that wants to castle  (mainly used for its team)
 * @param king_pos the current position of the king
 * @param step the direction from the king to the castle (i.e. 1 for king side and -1 for queen side)
 *
 * @note should be called only after it is certain that the path between the king and castling taking part in castling
 * is free \n (makes a copy of the board due to not wanting to const_cast the const qualifier away and end up with
 * unexpected behaviour)
 * @return false if the king would be in Check during castling, false otherwise
 */
static inline bool king_is_check_during_castling_move(const Board& board, Piece king, int8_t king_pos, int8_t step) {
#ifdef STANDARD_CHESS
  if (Check::isChecked(board, king.team, king_pos)) return true;
#else
    if (Check::actualAtomicCheck(board, king.team, king_pos)) return true;
#endif
  Board tempBoard = board;
  const int end = king_pos + 3 * step;
  Move move = {king, king_pos, -1, OptionalPieceType::nullopt(), SlimOptional<SpecialMove>::nullopt()};

  for (int current_pos = king_pos + step; current_pos != end; current_pos = current_pos + step) {
    move.to = static_cast<int8_t>(current_pos);
    tempBoard.movePiece(move);
    if (Check::actualAtomicCheck(tempBoard, king.team, move.to)) return true;
    move.from = move.to;
  }

  return false;
}

namespace PossibleMoves {

/**
 * @brief Generates all possible moves for a pawn on a specified square on a given board
 *
 * @param board The current state of the chess board
 * @param moveVec A vector to store the generated moves in
 * @param pawn The pawn for which to generate the possible moves (mainly used for its team)
 * @param pawn_pos The current position of the pawn on the board
 *
 * @note It considers normal moves, captures, and special moves like en passant and promotion.
 */
void getPossiblePawnMove(const Board& board, std::vector<Move>& moveVec, Piece pawn, int8_t pawn_pos) {
  const int8_t startLine = pawn.team == Team::WHITE ? 6 : 1;
  const int8_t pawnDir = pawn.team == Team::WHITE ? -1 : 1;
  const int8_t endLine = pawn.team == Team::WHITE ? 0 : 7;
  const Vec2 pawnMovement = Vec2(0, pawnDir);
  const Vec2 captureOffset[2] = {{-1, pawnDir}, {1, pawnDir}};
  const Vec2 pawn_pos_2D = Vec2::newVec2(pawn_pos);
  const bool isStartLine = pawn_pos_2D.y == startLine;
  const Team::Team enemyTeam = Team::getEnemyTeam(pawn.team);

  // basic move
  Move move = {pawn, pawn_pos, -1, OptionalPieceType::nullopt(), SlimOptional<SpecialMove>::nullopt()};

  // special for promotion
  auto addToMoveVecWithPossiblePromotion = [&moveVec, &move, endLine](int8_t to) {
    move.to = to;

    int8_t to_line = static_cast<int8_t>(to >> 3);
    if (to_line == endLine) {
      for (int8_t i = PieceType::PAWN - 1; i > PieceType::KING; --i) {
        move.promote = SlimOptional(static_cast<PieceType::PieceType>(i));
        moveVec.push_back(move);
      }
      move.promote = OptionalPieceType::nullopt();
    } else {
      moveVec.push_back(move);
    }
  };

  // possible normal Moves
  for (
      struct {
        int i;
        Vec2 to2D;
      } i = {0, pawn_pos_2D + pawnMovement};
      (i.i < (1 + isStartLine)) && !i.to2D.outsideBoard(); ++i.i, i.to2D += pawnMovement) {
    const int8_t to = i.to2D.getPos();
    if (board.grid[to].type != PieceType::NONE) break;

    addToMoveVecWithPossiblePromotion(to);

    move.specialMove = SlimOptional(SpecialMove::pawnMove2);
  }
  move.specialMove = SlimOptional<SpecialMove>::nullopt();

  // possible Capture Moves
  for (Vec2 pawns : captureOffset) {
    const Vec2 to2D = pawn_pos_2D + pawns;

    if (to2D.outsideBoard()) continue;

    const int8_t to = to2D.getPos();
    const Piece capturePiece = board.grid[to];

    const bool canCapture = enemyTeam == capturePiece.team;
#ifndef EN_PASSANT
    const bool enPassantExists = board.extra.getPassantPos().has_value();
#endif
    if (canCapture) {
      addToMoveVecWithPossiblePromotion(to);
    }
#ifndef EN_PASSANT
    if (enPassantExists) {
      const int8_t passantablePiecePos = board.extra.getPassantPos().data;
      const bool enPassantCorrectPos = (to == passantablePiecePos);
      const int8_t piece_having_done_passant_pos = static_cast<int8_t>(move.from + pawns.x);

      if (enPassantCorrectPos && (enemyTeam == board.grid[piece_having_done_passant_pos].team)) {
        move.to = to;
        move.specialMove = SlimOptional(SpecialMove::en_Passant);
        moveVec.push_back(move);
        move.specialMove = SlimOptional<SpecialMove>::nullopt();
      }
    }
#endif
  }
}

/**
 * @brief Generates all possible moves for a king on a specified square on a given board
 *
 * @param board The current state of the chess board
 * @param moveVec A vector to store the generated moves in
 * @param king The king for which to generate the possible moves (mainly used for its team)
 * @param king_pos The current position of the king on the board
 *
 * @note It considers normal moves, captures, and castling.
 */
void getPossibleKingMove(const Board& board, std::vector<Move>& moveVec, Piece king, int8_t king_pos) {
  constexpr Vec2 kingOffset[8] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}, {0, -1}, {0, 1}, {-1, 0}, {1, 0}};

  Move move = {king, king_pos, 0, OptionalPieceType::nullopt(), SlimOptional(SpecialMove::loseCastlingBoth)};
  Vec2 king_pos_2D = Vec2::newVec2(king_pos);

  for (Vec2 offset : kingOffset) {
    ChessPos to = check_if_possible_move(board, king, king_pos_2D, offset);
    if (!to.has_value()) continue;
    move.to = to.data;
    moveVec.push_back(move);
  }
#ifndef CASTLING
  const int8_t teamOffset = king.team == Team::WHITE ? ChessConstants::start_white_queen_side_castle_pos
                                                     : ChessConstants::start_black_queen_side_castle_pos;
  // check if king can castle Queen Side
  int8_t castle_pos = static_cast<int8_t>(0 + teamOffset);
  if (board.extra.getCastlingRights(castle_pos) &&
      path_is_free_between_king_and_castle(board, king_pos, castle_pos, -1) &&
      !king_is_check_during_castling_move(board, king, king_pos, -1)) {
    move.to = static_cast<int8_t>(king_pos - 2);
    move.specialMove = SlimOptional(SpecialMove::CastleQueenSide);
    moveVec.push_back(move);
  }

  // check if king can castle King Side
  castle_pos = static_cast<int8_t>(7 + teamOffset);
  if (board.extra.getCastlingRights(castle_pos) &&
      path_is_free_between_king_and_castle(board, king_pos, castle_pos, 1) &&
      !king_is_check_during_castling_move(board, king, king_pos, 1)) {
    move.to = static_cast<int8_t>(king_pos + 2);
    move.specialMove = SlimOptional(SpecialMove::CastleKingSide);
    moveVec.push_back(move);
  }
#endif
}

/**
 * @brief Generates all possible moves for a knight on a specified square on a given board
 *
 * @param board The current state of the chess board
 * @param moveVec A vector to store the generated moves in
 * @param knight The knight for which to generate the possible moves (mainly used for its team)
 * @param knight_pos The current position of the knight on the board
 */
void getPossibleKnightMove(const Board& board, std::vector<Move>& moveVec, Piece knight, int8_t knight_pos) {
  constexpr Vec2 knightOffsets[8] = {{-2, -1}, {-2, 1}, {2, -1}, {2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}};

  Move move = {knight, knight_pos, 0, OptionalPieceType::nullopt(), SlimOptional<SpecialMove>::nullopt()};
  const Vec2 knight_pos_2D = Vec2::newVec2(knight_pos);

  for (Vec2 offset : knightOffsets) {
    ChessPos to = check_if_possible_move(board, knight, knight_pos_2D, offset);
    if (!to.has_value()) continue;
    move.to = to.data;
    moveVec.push_back(move);
  }
}

/**
 * @brief Generates all possible moves for a castle on a specified square on a given board
 *
 * @param board The current state of the chess board
 * @param moveVec A vector to store the generated moves in
 * @param castle The castle for which to generate the possible moves (mainly used for its team)
 * @param castle_pos The current position of the castle on the board
 * @param specialMove the lost castling rights due to moving this specific castle
 */
void getPossibleCastleMove(const Board& board, std::vector<Move>& moveVec, Piece castle, int8_t castle_pos,
                           SlimOptional<SpecialMove> specialMove) {
  constexpr Vec2 lines[4] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

  Move move = {castle, castle_pos, 0, OptionalPieceType::nullopt(), specialMove};
  auto forEach = [&moveVec, &move](int8_t to) {
    move.to = to;
    moveVec.push_back(move);
  };

  for (Vec2 line : lines) {
    get_possible_move_linear_search(board, forEach, castle, Vec2::newVec2(castle_pos), line);
  }
}

/**
 * @brief Generates all possible moves for a bishop on a specified square on a given board
 *
 * @param board The current state of the chess board
 * @param moveVec A vector to store the generated moves in
 * @param bishop The bishop for which to generate the possible moves (mainly used for its team)
 * @param bishop_pos The current position of the bishop on the board
 */
void getPossibleBishopMove(const Board& board, std::vector<Move>& moveVec, Piece bishop, int8_t bishop_pos) {
  constexpr Vec2 diags[4] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}};

  Move move = {bishop, bishop_pos, 0, OptionalPieceType::nullopt(), SlimOptional<SpecialMove>::nullopt()};
  auto forEach = [&moveVec, &move](int8_t to) {
    move.to = to;
    moveVec.push_back(move);
  };

  for (Vec2 diag : diags) {
    get_possible_move_linear_search(board, forEach, bishop, Vec2::newVec2(bishop_pos), diag);
  }
}

/**
 * @brief Generates all possible moves for a queen on a specified square on a given board
 *
 * @param board The current state of the chess board
 * @param moveVec A vector to store the generated moves in
 * @param queen The queen for which to generate the possible moves (mainly used for its team)
 * @param queen_pos The current position of the queen on the board
 */
void getPossibleQueenMove(const Board& board, std::vector<Move>& moveVec, Piece queen, int8_t queen_pos) {
  getPossibleCastleMove(board, moveVec, queen, queen_pos, SlimOptional<SpecialMove>::nullopt());
  getPossibleBishopMove(board, moveVec, queen, queen_pos);
}

// (function documentation is provided in the corresponding header)
void getAllPossibleMoves(const Board& board, std::vector<Move>& moveVec, Team::Team team) {
  // all king moves
  Board_Positions::PositionsSlice kings = board.positions.getVec(team, PieceType::KING);
  for (int i = 0; i < kings.size; ++i) {
    getPossibleKingMove(board, moveVec, Piece(team, PieceType::KING), kings[i]);
  }

  // all queen moves
  Board_Positions::PositionsSlice queens = board.positions.getVec(team, PieceType::QUEEN);
  for (int i = 0; i < queens.size; ++i) {
    int8_t queenPos = queens[i];
    getPossibleQueenMove(board, moveVec, Piece(team, PieceType::QUEEN), queenPos);
  }

  // all bishop moves
  Board_Positions::PositionsSlice bishops = board.positions.getVec(team, PieceType::BISHOP);
  for (int i = 0; i < bishops.size; ++i) {
    int8_t bishopPos = bishops[i];
    getPossibleBishopMove(board, moveVec, Piece(team, PieceType::BISHOP), bishopPos);
  }

  // all knight moves
  Board_Positions::PositionsSlice knights = board.positions.getVec(team, PieceType::KNIGHT);
  for (int i = 0; i < knights.size; ++i) {
    int8_t knightPos = knights[i];
    getPossibleKnightMove(board, moveVec, Piece(team, PieceType::KNIGHT), knightPos);
  }

  // all castle moves
  Board_Positions::PositionsSlice castles = board.positions.getVec(team, PieceType::CASTLE);
  for (int i = 0; i < castles.size; ++i) {
    int8_t castlePos = castles[i];
    uint8_t castlingRights = board.extra.getCastlingRights(castlePos);
    SlimOptional<SpecialMove> special = SlimOptional(static_cast<SpecialMove>(castlingRights >> (team << 1)));

    getPossibleCastleMove(board, moveVec, Piece(team, PieceType::CASTLE), castlePos, special);
  }

  // all pawn moves
  Board_Positions::PositionsSlice pawns = board.positions.getVec(team, PieceType::PAWN);
  for (int i = 0; i < pawns.size; ++i) {
    int8_t pawnPos = pawns[i];
    getPossiblePawnMove(board, moveVec, Piece(team, PieceType::PAWN), pawnPos);
  }
}
}  // namespace PossibleMoves
