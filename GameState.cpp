#include "GameState.h"

const static std::string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

GameState::GameState() {
    chessBoard = new ChessBoard(startingFen);
    gui = new ChessGUI(); 
    gameState = PLAYING_GAME;
    moveGenerator = new MoveGenerator();
}

void GameState::start() {
    while (gameState != QUIT) {
        if (gameState == PLAYING_GAME) {
            gameLoop();
        } else if (gameState == GAME_OVER) {
            gameOver();
        }
    }
    gui->quitChessGUI();
    delete chessBoard;
    delete gui;
    delete moveGenerator;
}

void GameState::gameLoop() {
    SDL_Event ev;
    Move move;
    int selectedIndex = -1;
    int endIndex = -1;
    int index, x, y, from;
    Move moves[MAXMOVES];
    Move* end = moveGenerator->generateMoves(chessBoard, moves);
    while (gameState == PLAYING_GAME) {
        while(SDL_PollEvent(&ev) != 0) {
            if (ev.type == SDL_QUIT) {
                gameState = QUIT;
                break;
            } else if (ev.type == SDL_MOUSEBUTTONDOWN) {
                SDL_GetMouseState(&x, &y);
                index = gui->screenCoordToBoardIndex(x, y);
                if (index >= 0 && chessBoard->pieceOn(index)) {
                    selectedIndex = index;
                    from = selectedIndex;
                }
            } else if (ev.type == SDL_MOUSEBUTTONUP) {
                SDL_GetMouseState(&x, &y);
                index = gui->screenCoordToBoardIndex(x, y);
                if (index >= 0 && selectedIndex >= 0 && index != selectedIndex) {
                    endIndex = index;
                    move = makeMove<NOFLAG>(from, endIndex, chessBoard->pieceOn(endIndex), EMPTY);
                    chessBoard->makeMove(move);
                } //TODO: filter illegal move
                selectedIndex = -1;
            }
            gui->drawBoard(chessBoard, selectedIndex);
        }
    }
}

void GameState::gameOver() {

}