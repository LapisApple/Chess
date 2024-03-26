

G++ = g++

G++_FLAGS = -std=c++17 -c -O3 -Wall -Wextra -Wpedantic -DNO_ASSERTS
LD_FLAGS = -L /usr/local/lib -l pthread

OBJECTS =  main.o PossibleMoves.o Print.o FenParsing.o Check.o MinMax.o Evaluate.o MainGameLoop.o
TARGET = Chess

vpath Check.cpp Check
vpath Evaluate.cpp Bot/Evaluation
vpath MinMax.cpp Bot/MinMax
vpath PossibleMoves.cpp PossibleMoves
vpath FenParsing.cpp IO/Parser
vpath Print.cpp IO/Print

all: $(TARGET)

$(TARGET): $(OBJECTS)
	g++ -o $(TARGET) $(OBJECTS) $(LD_FLAGS)

%.o : %.cpp
	$(G++) $(G++_FLAGS) $<

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean
