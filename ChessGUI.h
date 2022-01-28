#ifndef CHESSGUI_H
#define CHESSGUI_H

#include "SDL.h"
#include "SDL_image.h"
#include "ChessBoard.h"
#include "Types.h"
#include <iostream>
#include <unordered_map>

#define WHITER 238
#define WHITEG 238
#define WHITEB 210
#define BLACKR 118
#define BLACKG 150
#define BLACKB 86
#define CHERRYR 210
#define CHERRYG 4
#define CHERRYB 45
#define WOODR 186
#define WOODG 140
#define WOODB 99

class ChessGUI {
    public:
    ChessGUI();
    // Draws a board with a selected square and its highlighted moves
    // To draw a board with no selection and move highlighting pass -1 as selected index and
    // and int array of 64 zeroes
    void drawBoard(ChessBoard* board, int selectedIndex, int* movMat);
    void drawPromSelection(Colour colour);
    void quitChessGUI();
    int screenCoordToBoardIndex(int x, int y);

    int width;
    int height;

    private:
    void loadTexture();
    void drawPiece(SDL_Rect rect, Piece piece);

    SDL_Window* window;
    SDL_Renderer* renderer;

    std::unordered_map<Piece, SDL_Texture*> pieceTextures;
};

#endif