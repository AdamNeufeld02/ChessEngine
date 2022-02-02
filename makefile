EXE = ChessEngine
EXET = ChessTest
EXEP = Perft

#OBJS specifies which files to compile as part of the project
OBJS_EXE = main.o ChessGUI.o GameState.o MoveGenerator.o ChessBoard.o BitBoards.o Evaluation.o Search.o MovePick.o Zobrist.o Misc.o
OBJS_EXET =  Test.o MoveGenerator.o ChessBoard.o BitBoards.o Evaluation.o Search.o MovePick.o Zobrist.o Misc.o
OBJS_EXEP = Perft.o MoveGenerator.o ChessBoard.o BitBoards.o Evaluation.o Zobrist.o Misc.o

#CC specifies which compiler we're using
CC = g++

CXX_FLAGS = -c -g -Ofast -Wall -Wextra -pedantic
#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -IC:\Users\adamn\SDL2\include\SDL2

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -LC:\Users\adamn\SDL2\lib

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
#COMPILER_FLAGS = -w -Wl,-subsystem,windows

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = ChessEngine

all : ChessEngine ChessTest Perft

#This is the target that compiles our executable
$(EXE) : $(OBJS_EXE)
	$(CC) $(OBJS_EXE) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) -o $(EXE)

$(EXET) : $(OBJS_EXET)
	$(CC) $(OBJS_EXET) -o $(EXET)

$(EXEP) : $(OBJS_EXEP)
	$(CC) $(OBJS_EXEP) -o $(EXEP)

MoveGenerator.o : MoveGenerator.cpp MoveGenerator.h ChessBoard.h Types.h BitBoards.h
	$(CC) MoveGenerator.cpp $(CXX_FLAGS) -o $@

ChessGUI.o : ChessGUI.cpp ChessGUI.h Types.h ChessBoard.h BitBoards.h Evaluation.h
	$(CC) ChessGUI.cpp $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) $(CXX_FLAGS) -o $@

GameState.o : GameState.cpp GameState.h ChessGUI.h MoveGenerator.h ChessBoard.h BitBoards.h Evaluation.h
	$(CC) GameState.cpp $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) $(CXX_FLAGS) -o $@

main.o : main.cpp GameState.h
	$(CC) main.cpp $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) $(CXX_FLAGS) -o $@

Test.o : Test.cpp MoveGenerator.h ChessBoard.h BitBoards.h Search.h Evaluation.h MovePick.h
	$(CC) Test.cpp $(CXX_FLAGS) -o $@

Perft.o : Perft.cpp MoveGenerator.h ChessBoard.h BitBoards.h
	$(CC) Perft.cpp $(CXX_FLAGS) -o $@

ChessBoard.o : ChessBoard.cpp ChessBoard.h Types.h BitBoards.h Evaluation.h
	$(CC) ChessBoard.cpp $(CXX_FLAGS) -o $@

BitBoards.o : BitBoards.cpp BitBoards.h
	$(CC) BitBoards.cpp $(CXX_FLAGS) -o $@

Evaluation.o : Evaluation.cpp Evaluation.h ChessBoard.h
	$(CC) Evaluation.cpp $(CXX_FLAGS) -o $@

Search.o : Search.cpp Search.h Types.h MoveGenerator.h Evaluation.h MovePick.h
	$(CC) Search.cpp $(CXX_FLAGS) -o $@

MovePick.o : MovePick.cpp MovePick.h Types.h ChessBoard.h
	$(CC) MovePick.cpp $(CXX_FLAGS) -o $@ 

Misc.o : Misc.cpp Misc.h BitBoards.h
	$(CC) Misc.cpp $(CXX_FLAGS) -o $@

Zobrist.o : Zobrist.cpp Zobrist.h Types.h Misc.h
	$(CC) Zobrist.cpp $(CXX_FLAGS) -o $@

