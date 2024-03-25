# Makefile for gtest

GOOGLE_TEST_LIB = gtest
GOOGLE_TEST_INCLUDE = /usr/local/include

G++ = g++

G++_FLAGS = -std=c++17 -c -O3 -Wall -Wextra -Wpedantic -I $(GOOGLE_TEST_INCLUDE)
LD_FLAGS = -L /usr/local/lib -l $(GOOGLE_TEST_LIB) -l pthread

OBJECTS = main.o BasicPieceLogic.o correctConstants.o StandardPerftTest.o Check.o PossibleMoves.o FenParsing.o Print.o UndoPerftTest.o ParsingTest.o EvalTest.o Evaluate.o
TARGET = Test

vpath Check.cpp ../../src/Check
vpath Evaluate.cpp ../../src/Bot/Evaluation
vpath PossibleMoves.cpp ../../src/PossibleMoves
vpath FenParsing.cpp ../../src/IO/Parser
vpath Print.cpp ../../src/IO/Print

all: $(TARGET)

$(TARGET): $(OBJECTS)
	g++ -o $(TARGET) $(OBJECTS) $(LD_FLAGS)

%.o : %.cpp
	$(G++) $(G++_FLAGS) $<

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean
