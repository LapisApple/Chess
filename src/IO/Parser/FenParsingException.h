//
// Created by timap on 19.03.2024.
//

#pragma once
#include <exception>

/**
 * @brief Base exception class for FEN parsing errors.
 * \n \n
 * This class provides a general error message for FEN parsing errors.
 * It is intended to be inherited by more specific exception classes.
 */
class FenParsingException : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "Some Problems Occurred while Parsing the given Fen. \nPlease Try a different Fen";
  }
};

/**
 * @brief Exception class for missing data in FEN.
 * \n \n
 * This class provides an error message for cases where the FEN string is missing necessary data.
 */
class MissingDataInFen : public FenParsingException {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "Data is Missing In The Given Fen. \nPlease add it or change to a different Fen";
  }
};

/**
 * @brief Exception class for FEN Reader going unexpectedly out of bounds.
 * \n \n
 * as said in the message this should not be thrown, but exists just to be safe.
 */
class InternalOutOfBoundsErrorFen : public FenParsingException {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "This Error should only be thrown when an unexpected Situation occurred or the given Fen is explicitly "
           "trying to break the Fen Reader. If you believe the Fen is actually legal open an issue on gitlab or "
           "github.\n"
           "The Fen Reader tried to access out of bounds of the given, probably illegal, Fen. \nPlease add the missing "
           "data, or add some trailing whitespace / other character, or simply change to a different Fen";
  }
};

// Fen Board
/**
 * @brief Exception class for illegal characters in the FEN board part.
 */
class IllegalCharacterInFenBoard : public FenParsingException {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "An Illegal Character has been found in the Board part of the Fen \nPlease remove it or change to a "
           "different Fen";
  }
};

/**
 * @brief Exception class for illegal amount of pieces and empty squares in a row in the FEN board part.
 */
class IllegalAmountPiecesOnARow : public FenParsingException {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "An Illegal Amount of Pieces and Whitespace has been found on A Row  \nPlease Try a different Fen";
  }
};

/**
 * @brief Exception class for illegal amount of rows in the FEN board part.
 */
class IllegalAmountOfRowsInBoard : public FenParsingException {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "An Illegal Amount of Rows has been found in the board part of the Fen  \nPlease Try a different Fen";
  }
};

/**
 * @brief Exception class for illegal amount of pieces in 1 or more teams in the FEN board part.
 */
class IllegalAmountOfPiecesInTeam : public FenParsingException {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "An Illegal Amount of Pieces has been found in one or more of the Teams \nPlease Try a different Fen";
  }
};

/**
 * @brief Exception class for illegal amount of kings in 1 or more teams in the FEN board part.
 */
class IllegalAmountOfKingsInTeam : public FenParsingException {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "An Illegal Amount of Kings has been found in one or more of the Teams \nPlease Try a different Fen";
  }
};

// Fen Castling
/**
 * @brief Exception class for illegal castling rights in 1 or more teams in the FEN castling part.
 */
class IllegalCastlingRightsInFen : public FenParsingException {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "Illegal Castling Rights have been found in the Fen \nPlease Try a different Fen";
  }
};

/**
 * @brief Exception class for missing castling rights part in the FEN.
 */
class MissingCastlingRightsInFen : public FenParsingException {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "Castling Rights are missing in the Fen \nPlease Try a different Fen";
  }
};

// Fen En Passant
/**
 * @brief Exception class for illegal en Passant Square in the FEN en Passant part.
 */
class IllegalEnPassantPositionInFen : public FenParsingException {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "An Illegal En Passant Position has been found in the Fen \nPlease Try a different Fen";
  }
};

/**
 * @brief Exception class for missing en Passant part in the FEN.
 */
class MissingEnPassantDataInFen : public FenParsingException {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "En Passant Data is missing in the Fen \nPlease Try a different Fen";
  }
};

// Fen Turn Data
/**
 * @brief Exception class for missing Turn data part in the FEN.
 */
class MissingTurnDataInFen : public FenParsingException {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "Full Turn Data is missing in the Fen \nPlease Try a different Fen";
  }
};

/**
 * @brief Exception class for missing Half Turn data since pawn move or capture part in the FEN.
 */
class MissingHalfTurnDataInFen : public FenParsingException {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "Half Turn Data is missing in the Fen \nPlease Try a different Fen";
  }
};

/**
 * @brief Exception class for missing current player turn (which player moves next) in the FEN.
 */
class MissingCurrentPlayerDataInFen : public FenParsingException {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "Current Player Color is missing in the Fen \nPlease Try a different Fen";
  }
};
