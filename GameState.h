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
    ChessBoard* chessBoard;
    ChessGUI* gui;
    MoveGenerator* moveGenerator;
};

#endif