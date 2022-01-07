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
    int movMat[64];
    Move* end = moveGenerator->generateMoves(chessBoard, moves);
    resetMoveMatrix(movMat);
    while (gameState == PLAYING_GAME) {
        while(SDL_PollEvent(&ev) != 0) {
            if (ev.type == SDL_QUIT) {
                gameState = QUIT;
                break;
            } else if (ev.type == SDL_MOUSEBUTTONDOWN) {
                SDL_GetMouseState(&x, &y);
                index = gui->screenCoordToBoardIndex(x, y);
                makeMoveMatrix(moves, end, index, movMat);
                if (index >= 0 && chessBoard->pieceOn(index)) {
                    selectedIndex = index;
                    from = selectedIndex;
                }
            } else if (ev.type == SDL_MOUSEBUTTONUP) {
                SDL_GetMouseState(&x, &y);
                index = gui->screenCoordToBoardIndex(x, y);
                if (index >= 0 && selectedIndex >= 0 && index != selectedIndex) {
                    endIndex = index;
                    move = findMove(moves, end, from, endIndex);
                    if (move.captToFrom) {
                        chessBoard->makeMove(move);
                        end = moveGenerator->generateMoves(chessBoard, moves);
                    }
                } //TODO: filter illegal move
                selectedIndex = -1;
                resetMoveMatrix(movMat);
            }
            gui->drawBoard(chessBoard, selectedIndex, movMat);
        }
    }
}

Move GameState::findMove(Move* begin, Move* end, int from, int to) {
    int size = end - begin;
    for (int i = 0; i < size; i++) {
        Move move = begin[i];
        if (getFrom(move) == from && getTo(move) == to) {
            return move;
        }
    }
    return makeMove<NOFLAG>(0, 0, EMPTY, EMPTY);
}

void GameState::resetMoveMatrix(int* movMat) {
    for (int i = 0; i < 64; i++) movMat[i] = 0;
}

void GameState::makeMoveMatrix(Move* begin, Move* end, int index, int* movMat) {
    int size = end - begin;
    // init movMat to 0
    resetMoveMatrix(movMat);
    // Find moves and set squares in matrix
    for (int i = 0; i < size; i++) {
        if (getFrom(begin[i]) == index) {
            movMat[getTo(begin[i])] += 1;
        }
    }
}

void GameState::gameOver() {

}