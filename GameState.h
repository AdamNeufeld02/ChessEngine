#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "ChessBoard.h"
#include "ChessGUI.h"
#include "MoveGenerator.h"
#include "SDL.h"
#include <iostream>

class GameState {
    public:
    static const int PLAYING_GAME = 1;
    static const int QUIT = 0;
    static const int GAME_OVER = -1;
    int gameState;

    GameState();
    void start();

    private:
    void gameLoop();
    void gameOver();
    // Builds a matrix where the destination of any move from the index provided
    // is set to one. Used in the ChessGui to highlight squares of possible moves
    void makeMoveMatrix(Move* begin, Move* end, int index, int* movMat);
    // Resets the move matrix to be blank
    void resetMoveMatrix(int* movMat);
    // Finds a move from the list of moves with the same start and end index
    // If multiple Moves returns first one
    // Returns empty move if no move found
    Move findMove(Move* begin, Move* end, int from, int to);
    ChessBoard* chessBoard;
    ChessGUI* gui;
    MoveGenerator* moveGenerator;
};

#endif