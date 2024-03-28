cmake_minimum_required(VERSION 3.26)
project(Chess2)

set(CMAKE_CXX_STANDARD 17)

add_executable(Chess2
        src/Board/Board_impl/Board_8x8.h
        src/Board/Board_impl/Board_Positions.h
        src/Board/Board_impl/Board_Extra.h
        src/Board/Board.h
        src/main.cpp
        src/Types/Vec2.h
        src/Types/SlimOptional.h
        src/Types/Move/Move.h
        src/Types/BasicChessTypes.h
        src/IO/Parser/FenParsing.cpp
        src/IO/Parser/FenParsing.h
        src/PossibleMoves/PossibleMoves.h
        src/PossibleMoves/PossibleMoves.cpp
        src/Check/Check.h
        src/Check/Check.cpp
        src/Bot/Evaluation/Evaluate.h
        src/Bot/MinMax/MinMax.h
        src/Bot/MinMax/MinMax.cpp
        src/Check/CheckMate.h
        src/ChessGame.h
        src/IO/Print/Print.h
        src/IO/Print/Print.cpp
        src/IO/Parser/StringParsing.h
        src/Board/Board_Correctness.h
        src/IO/Parser/FenParsingException.h
        src/Types/ChessConstants.h
        src/Types/Move/ExecutedMove.h
        src/Types/Move/UserMove.h
        src/Bot/Evaluation/pesto_tables.h
        src/Bot/Evaluation/Evaluate.cpp
        src/MainGameLoop.cpp
        src/MainGameLoop.h
        src/AdjacentPieces.h
)