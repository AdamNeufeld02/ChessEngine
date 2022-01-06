EXE = ChessEngine
EXET = ChessTest

#OBJS specifies which files to compile as part of the project
OBJS_EXE = main.o ChessGUI.o GameState.o MoveGenerator.o ChessBoard.o
OBJS_EXET =  Test.o MoveGenerator.o ChessBoard.o

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

all : ChessEngine ChessTest

#This is the target that compiles our executable
$(EXE) : $(OBJS_EXE)
	$(CC) $(OBJS_EXE) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) -o $(EXE)

$(EXET) : $(OBJS_EXET)
	$(CC) $(OBJS_EXET) -o $(EXET)

MoveGenerator.o : MoveGenerator.cpp MoveGenerator.h ChessBoard.h Types.h
	$(CC) MoveGenerator.cpp $(CXX_FLAGS) -o $@

ChessGUI.o : ChessGUI.cpp ChessGUI.h Types.h ChessBoard.h
	$(CC) ChessGUI.cpp $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) $(CXX_FLAGS) -o $@

GameState.o : GameState.cpp GameState.h ChessGUI.h MoveGenerator.h ChessBoard.h
	$(CC) GameState.cpp $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) $(CXX_FLAGS) -o $@

main.o : main.cpp GameState.h
	$(CC) main.cpp $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) $(CXX_FLAGS) -o $@

Test.o : Test.cpp MoveGenerator.h ChessBoard.h
	$(CC) Test.cpp $(CXX_FLAGS) -o $@

ChessBoard.o : ChessBoard.cpp ChessBoard.h Types.h
	$(CC) ChessBoard.cpp $(CXX_FLAGS) -o $@

