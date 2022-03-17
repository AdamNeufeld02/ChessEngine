
BUILD_DIR = build
ENG_DIR = src/engine
TST_DIR = src/test
GUI_DIR = src/gui

EXE = $(BUILD_DIR)/ChessGUI
EXET = $(BUILD_DIR)/ChessTest

# Get all of the source files
SRCS_ENG := $(wildcard $(ENG_DIR)/*.cpp)
SRCS_TST := $(wildcard $(TST_DIR)/*.cpp)
SRCS_GUI := $(wildcard $(GUI_DIR)/*.cpp)

OBJS_ENG := $(patsubst $(ENG_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS_ENG))
OBJS_TST := $(patsubst $(TST_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS_TST))
OBJS_GUI := $(patsubst $(GUI_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS_GUI))

#CC specifies which compiler we're using
CXX = g++

CXX_FLAGS = -c -g -Ofast -Wall -Wextra -pedantic

INCLUDE_ENGINE = -Isrc/engine

INCLUDE_SDL = -IC:\msys64\mingw64\include\SDL2

LIBRARY_PATHS = -LC:\Users\adamn\SDL2\lib

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

all : $(EXE) $(EXET)

#Rule for GUI exe
$(EXE) : $(OBJS_ENG) $(OBJS_GUI)
	$(CXX) $(OBJS_ENG) $(OBJS_GUI) $(LIBRARY_PATHS) $(LINKER_FLAGS) -o $(EXE)

#Rule for Test exe
$(EXET) : $(OBJS_ENG) $(OBJS_TST)
	$(CXX) $(OBJS_ENG) $(OBJS_TST) -o $(EXET)

#Pattern for Engine objects
$(BUILD_DIR)/%.o: $(ENG_DIR)/%.cpp
	$(CXX) $(CXX_FLAGS) $(ENG_DIR)/$*.cpp -o $@

#Pattern for GUI objects
$(BUILD_DIR)/%.o: $(GUI_DIR)/%.cpp
	$(CXX) $(INCLUDE_SDL) $(INCLUDE_ENGINE) $(CXX_FLAGS) $(GUI_DIR)/$*.cpp -o $@

#Patter for Test objects
$(BUILD_DIR)/%.o: $(TST_DIR)/%.cpp
	$(CXX) $(INCLUDE_ENGINE) $(CXX_FLAGS) $(TST_DIR)/$*.cpp -o $@

clean:
	del $(BUILD_DIR)\*.o
	del $(BUILD_DIR)\*.exe
