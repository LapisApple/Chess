//
// Created by timap on 27.01.2024.
//

#include "FenParsing.h"
#include "StringParsing.h"
#include "../../ChessConstants.h"
#include "../../Board/Board_impl/Board_Extra.h"

/**
 * @brief Processes the board part of a FEN string.
 *
 * @param reader A FenStringReader object for reading the FEN string,
 * that is currently pointing at the start of the board part of the FEN.
 * @return The length of the board part of the FEN \n
 * (moves the reader forward past the board part of the FEN)
 *
 * @note also validates that : \n
 * - both kings exist \n
 * - each team has \<=16 pieces \n
 * - there are no unexpected chars in the board \n
 * - each row describes exactly 8 squares \n
 * - there are exactly 7 '/' corresponding to 8 total rows \n
 */
constexpr int processBoard(FenStringReader& reader) {
    const int beginning_offset = reader.getCurrentOffset();

    int amount_kings_per_team[TEAM_AMOUNT] = {0, 0};
    int amount_of_pieces_per_team[TEAM_AMOUNT] = {0, 0};
    int amountSlashesInFenString = 0;
    int piecesPerRow = 0;

    for (; reader.hasCharsLeft(); ++reader) {
        if (reader.currentCharIsWhiteSpace()) break;

        char ch = reader.get();
        switch (ch) {
            case 'k':
                amount_kings_per_team[Team::BLACK]++;
            case 'q':
            case 'b':
            case 'r':
            case 'n':
            case 'p':
                amount_of_pieces_per_team[Team::BLACK]++;
                ch = '1';
                break;
            case 'K':
                amount_kings_per_team[Team::WHITE]++;
            case 'Q':
            case 'B':
            case 'R':
            case 'N':
            case 'P':
                amount_of_pieces_per_team[Team::WHITE]++;
                ch = '1';
                break;
            case '/':
                amountSlashesInFenString++;
                if (piecesPerRow != 8) throw IllegalAmountPiecesOnARow();
                piecesPerRow = 0;
                ch = '0';
                break;
            default:
                if (ch < '1' || ch > '8') throw IllegalCharacterInFenBoard();
                break;
        }
        piecesPerRow += charToInt(ch);
    }
    if (piecesPerRow != 8) throw IllegalAmountPiecesOnARow();
    if (amountSlashesInFenString != 7) throw IllegalAmountOfRowsInBoard();
    if (amount_of_pieces_per_team[Team::BLACK] > PIECES_PER_TEAM || amount_of_pieces_per_team[Team::WHITE] > PIECES_PER_TEAM) throw IllegalAmountOfPiecesInTeam();
    if (amount_kings_per_team[Team::WHITE] != 1 || amount_kings_per_team[Team::BLACK] != 1) throw IllegalAmountOfKingsInTeam();

    return reader.getCurrentOffset() - beginning_offset;
}

/**
 * @brief Processes the current player part of a FEN string.
 *
 * @param reader A FenStringReader object reading the FEN string,
 * that is currently pointing at the start of the current player part of the FEN.
 * @return The current player that is allowed to execute the next move \n
 * (moves the reader forward past the board part of the FEN)
 *
 * @note
 * accepts both uppercase and lowercase chars for the current player
 * \n \n
 * also validates that : \n
 * - an actual current Player exists \n
 */
constexpr Team::Team processCurrentPlayer(FenStringReader& reader) {
    // safety measure to not access out of bounds
    if (!reader.hasCharsLeft()) throw MissingCurrentPlayerDataInFen();
    // basically preincrement the reader to not need to increment it in all cases if the switch case
    ++reader;
    switch (reader[-1]) {
        case 'w':
        case 'W':
            return Team::WHITE;
        case 'B':
        case 'b':
            return Team::BLACK;
        default:
            throw MissingCurrentPlayerDataInFen();
    }
}

/**
 * @brief Processes the castling part of a FEN string.
 *
 * @param reader A FenStringReader object reading the FEN string,
 * that is currently pointing at the start of the castling part of the FEN.
 * @return The castling flags in a uint8_t as needed by Board_Extra \n
 * (moves the reader forward past the board part of the FEN)
 *
 * @note also validates that : \n
 * - Castling rights have actually been specified \n
 */
constexpr uint8_t processCastling (FenStringReader& reader) {
    uint8_t res = 0;
    for (; reader.hasCharsLeft(); ++reader) {
        if (reader.currentCharIsWhiteSpace()) break;

        switch (reader.get()) {
            case 'Q':
                res |= 0b10 << (Team::WHITE << 1);
                break;
            case 'K':
                res |= 0b01 << (Team::WHITE << 1);
                break;
            case 'q':
                res |= 0b10 << (Team::BLACK << 1);
                break;
            case 'k':
                res |= 0b01 << (Team::BLACK << 1);
                break;
            case '-':
                // the reader needs to move 1 forward to no longer be on castling data after processing castling
                ++reader;
                return 0b0000;
            default:
                throw IllegalCastlingRightsInFen();
        }
    }
    if (res == 0)  throw MissingCastlingRightsInFen();

    return res;
}

/**
 * @brief Processes the en passant part of a FEN string.
 *
 * @param reader A FenStringReader object reading the FEN string,
 * that is currently pointing at the start of the en passant part of the FEN.
 * @return the en Passant Position as an optional \n
 * (moves the reader forward past the board part of the FEN)
 *
 * @note also validates that : \n
 * - en Passant has actually been specified \n
 * - the row for en passant is correct \n
 */
constexpr ChessPos processEnPassant(FenStringReader& reader) {
    if (!reader.hasCharsLeft()) throw MissingEnPassantDataInFen();

    if (reader.get() == '-') {
        ++reader;
        return ChessPos::nullopt();
    }

    if (reader.amountCharsLeft() < 2) throw MissingEnPassantDataInFen();

    ChessPos pos = getPosFromChar(reader[0], reader[1]);

    if (!pos.has_value()) throw MissingEnPassantDataInFen();

    int row = pos.data >> 3;
    if (row != ChessConstants::black_en_passant_row && row != ChessConstants::white_en_passant_row)
        throw IllegalEnPassantPositionInFen();

    reader += 2;
    return pos;

}

/**
 * @brief Processes the parts of the FEN represented purely by numbers.
 *
 * @param reader A FenStringReader object reading the FEN string,
 * that is currently pointing at the start of the current part of the FEN.
 *
 * @return the number that is written as digits in the FEN if it exists,
 * otherwise an empty optional \n
 * (moves the reader forward past the board current of the FEN)
 */
constexpr inline NaturalNumber fenStrToInt(FenStringReader& reader) {
    bool is_possible_number = false;
    int res = 0;
    for (; reader.hasCharsLeft(); ++reader) {
        char ch = reader.get();
        if (ch < '0' || ch > '9') break;
        res = res * 10 + charToInt(ch);
        is_possible_number = true;
    }

    return is_possible_number ? SlimOptional(res) : NaturalNumber::nullopt();
}


/**
* @brief Retrieves the piece on a specific field in the chess board.
*
* @param board The FEN string representation of the chess board (previously validated by processBoard())
* @param field The square to retrieve the piece from
* @return The piece on the specified field, or a space character if the field is empty
*/
constexpr char getPieceOnField(const std::string_view& board, int8_t field) {
    int current_field = 0;
    int i = 0;
    for (; (i < board.size()) && (current_field < field); ++i) {
        char ch = board[i];

        if (ch == '/') continue;
        if (ch < '1' || ch > '8') ch = '1';
        current_field += ch - '0';
    }
    if ((i < board.size()) && board[i] == '/') i++;

    const char ch = board[i];
    const bool piece_on_field_exists = (current_field == field) && (ch < '1' || ch > '8');
    return piece_on_field_exists ? ch : ' ';
}

/**
 * @brief Checks the processed castling rights for correctness.
 *
 * @param fen A Fen object representing a Chess Board in FEN notation
 *
 * @note validates that : \n
 * - the kings and castles are at the correct squares for the given castling rights \n
 */
constexpr inline void checkCastlingRights(const Fen& fen) {
    Board_Extra extra = Board_Extra(fen.castling, fen.en_passant);

    // check correct castling rights for Black
    if (extra.getCastlingRights(ChessConstants::start_black_king_pos) && getPieceOnField(fen.board, ChessConstants::start_black_king_pos) != 'k')
        throw IllegalCastlingRightsInFen();
    if (extra.getCastlingRights(ChessConstants::start_black_queen_side_castle_pos) && getPieceOnField(fen.board, ChessConstants::start_black_queen_side_castle_pos) != 'r')
        throw IllegalCastlingRightsInFen();
    if (extra.getCastlingRights(ChessConstants::start_black_king_side_castle_pos) && getPieceOnField(fen.board, ChessConstants::start_black_king_side_castle_pos) != 'r')
        throw IllegalCastlingRightsInFen();

    // check correct castling rights for Black
    if (extra.getCastlingRights(ChessConstants::start_white_king_pos) && getPieceOnField(fen.board, ChessConstants::start_white_king_pos) != 'K')
        throw IllegalCastlingRightsInFen();
    if (extra.getCastlingRights(ChessConstants::start_white_queen_side_castle_pos) && getPieceOnField(fen.board, ChessConstants::start_white_queen_side_castle_pos) != 'R')
        throw IllegalCastlingRightsInFen();
    if (extra.getCastlingRights(ChessConstants::start_white_king_side_castle_pos) && getPieceOnField(fen.board, ChessConstants::start_white_king_side_castle_pos) != 'R')
        throw IllegalCastlingRightsInFen();
}

/**
 * @brief Checks the processed en passant for correctness.
 *
 * @param fen A Fen object representing a Chess Board in FEN notation
 *
 * @note validates that : \n
 * - there is actually a pawn at the correct square for being capture by the given en passant \n
 * - that pawn on that square is the correct team \n
 */
constexpr inline void checkCorrectEnPassant(const Fen& fen) {
    Board_Extra extra = Board_Extra(fen.castling, fen.en_passant);

    ChessPos passantable_piece_pos = extra.getPosOfPassantablePiece();

    if (!passantable_piece_pos.has_value()) throw IllegalEnPassantPositionInFen();

    char passantable_piece = getPieceOnField(fen.board, passantable_piece_pos.data);

    int row = fen.en_passant.data >> 3;

    if(row == ChessConstants::black_en_passant_row && passantable_piece != 'p')
        throw IllegalEnPassantPositionInFen();
    if (row == ChessConstants::white_en_passant_row && passantable_piece != 'P')
        throw IllegalEnPassantPositionInFen();
}

// (function documentation is provided in the corresponding header)
Fen Fen::buildFenFromStr(std::string_view str) {
    FenStringReader reader = FenStringReader(str);
    Fen result = Fen();

    reader.skipWhiteSpaceAndExpectFurtherData();
    int beginning = reader.getCurrentOffset();
    result.board  = str.substr(beginning, processBoard(reader));

    reader.skipWhiteSpaceAndExpectFurtherData();
    result.current_player = processCurrentPlayer(reader);

    reader.skipWhiteSpaceAndExpectFurtherData();
    result.castling = processCastling(reader);

    reader.skipWhiteSpaceAndExpectFurtherData();
    result.en_passant = processEnPassant(reader);

    reader.skipWhiteSpaceAndExpectFurtherData();
    NaturalNumber current_half_turns = fenStrToInt(reader);
    if (!current_half_turns.has_value()) throw MissingHalfTurnDataInFen();
    result.amount_half_moves = current_half_turns.data;

    reader.skipWhiteSpaceAndExpectFurtherData();
    NaturalNumber current_turn = fenStrToInt(reader);
    if (!current_turn.has_value()) throw MissingTurnDataInFen();
    result.current_turn = current_turn.data;

    if (result.en_passant.has_value()) checkCorrectEnPassant(result);
    checkCastlingRights(result);

    return result;
}