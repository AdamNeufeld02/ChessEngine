#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "ChessBoard.h"
#include "ChessGUI.h"
#include "MoveGenerator.h"
#include "SDL.h"
#include "Search.h"
#include <iostream>
#include <deque>
#include <memory>
#include <stdlib.h>

struct Player {
    bool isHuman;
    Colour colour;
};

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
    void makeMoveMatrix(ScoredMove* begin, ScoredMove* end, int index, int* movMat);
    // Resets the move matrix to be blank
    void resetMoveMatrix(int* movMat);
    // Finds a move from the list of moves with the same start and end index
    // If multiple Moves returns first one
    // Returns empty move if no move found
    Move findMove(ScoredMove* begin, ScoredMove* end, int from, int to);
    std::unique_ptr<std::deque<StateInfo>> states;
    Piece getPromotionFromUser(Colour colour);
    Move getMoveFromUser();
    Move getMoveFromComp();
    struct Player getPlayerToMove();
    ChessBoard* chessBoard;
    ChessGUI* gui;
    struct Player player1;
    struct Player player2;
};

#endif