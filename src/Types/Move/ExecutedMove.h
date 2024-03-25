//
// Created by timap on 22.03.2024.
//

#pragma once
#include <utility>

#include "../../Board/Board_impl/Board_Extra.h"
#include "../BasicChessTypes.h"
#include "Move.h"

/**
 * @brief Represents an already executed move in a chess game
 */
struct ExecutedMove {
  /**
   * The Move that was executed
   */
  Move move;
  /**
   * The the state of Board_Extra (i.e. castling and en passant) before the move
   */
  Board_Extra extra;
  /**
   * An optional field that, if present, indicates the Piece that was captured during the move
   */
  SlimOptional<Piece> capture;

  /**
   * A default constructor that creates an impossible/illegal executed move
   */
  constexpr ExecutedMove() noexcept
      : move(SlimOptional<Move>::nullopt().data),
        extra(Board_Extra(0b0, ChessPos::nullopt())),
        capture(SlimOptional<Piece>::nullopt()) {}
};
