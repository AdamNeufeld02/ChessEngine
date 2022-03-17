#include "ChessGUI.h"

ChessGUI::ChessGUI() {
    window = NULL;
    width = 800;
    height = 800;
    /*
    * Initialises the SDL video subsystem (as well as the events subsystem).
    */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
    } else {

        /* Creates a SDL window */
        window = SDL_CreateWindow("Chess Engine", /* Title of the SDL window */
                        SDL_WINDOWPOS_UNDEFINED, /* Position x of the window */
                        SDL_WINDOWPOS_UNDEFINED, /* Position y of the window */
                        width, /* Width of the window in pixels */
                        height, /* Height of the window in pixels */
                        SDL_WINDOW_RESIZABLE); /* Additional flag(s) */

        /* Checks if window has been created; if not, exits program */
        if (window == NULL) {
            fprintf(stderr, "SDL window failed to initialise: %s\n", SDL_GetError());
        } else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            loadTexture();
        }
    }
}

void ChessGUI::drawBoard(ChessBoard* chessBoard, int selectedIndex, int* movMat) {
    Piece selectedPiece;
    if (selectedIndex >= 0) {
        selectedPiece = chessBoard->pieceOn(selectedIndex);
    } else {
        selectedPiece = EMPTY;
    }
    SDL_GetWindowSize(window, &width, &height);
    int boardSideLength = std::min(width, height);
    int squareDimension = boardSideLength/8;
    SDL_RenderClear(renderer);
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int x = file * squareDimension;
            int y = (7 - rank) * squareDimension;
            SDL_Rect rect;
            rect.x = x;
            rect.y = y;
            rect.w = squareDimension;
            rect.h = squareDimension;
            if ((file + rank) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, WHITER, WHITEG, WHITEB, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, BLACKR, BLACKG, BLACKB, 255);
            }
            SDL_RenderDrawRect(renderer, &rect);
            SDL_RenderFillRect(renderer, &rect);

            if (((file + (8 * rank)) == selectedIndex) | (movMat[file + (8 * rank)])) {
                SDL_SetRenderDrawColor(renderer, CHERRYR, CHERRYG, CHERRYB, 255);
                SDL_RenderDrawRect(renderer, &rect);
                SDL_RenderFillRect(renderer, &rect);
            }

            if ((file + (8 * rank)) != selectedIndex) {
                drawPiece(rect, chessBoard->pieceOn((rank * 8) + file));
            }
        }  
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    int x, y;
    SDL_GetMouseState(&x, &y);
    x -= squareDimension/2;
    y -= squareDimension/2; // center piece
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = squareDimension;
    rect.h = squareDimension;
    drawPiece(rect, selectedPiece);
    SDL_RenderPresent(renderer);
}

void ChessGUI::drawPromSelection(Colour colour) {
    SDL_GetWindowSize(window, &width, &height);
    int boardSideLength = std::min(width, height);
    int squareDimension = boardSideLength/8;
    int colourConst = colour << 3;
    SDL_Rect rect;
    rect.x = 3 * squareDimension;
    rect.y = 3 * squareDimension;
    rect.w = 2 * squareDimension;
    rect.h = 2 * squareDimension;
    SDL_Rect selectedRect;
    // Draw the background
    SDL_SetRenderDrawColor(renderer, WOODR, WOODG, WOODB, 255);
    SDL_RenderDrawRect(renderer, &rect);
    SDL_RenderFillRect(renderer, &rect);

    int x, y, index;
    SDL_GetMouseState(&x, &y);
    index = screenCoordToBoardIndex(x, y);
    if (index == 27 || index == 28 || index == 35 || index == 36) {
        x = index % 8;
        y = index / 8;
        selectedRect.x = x * squareDimension;
        selectedRect.y = (7 - y) * squareDimension;
        selectedRect.w = squareDimension;
        selectedRect.h = squareDimension;
        
        SDL_SetRenderDrawColor(renderer, CHERRYR, CHERRYG, CHERRYB, 255);
        SDL_RenderDrawRect(renderer, &selectedRect);
    }
    
    
    
    for (int i = KNIGHT; i < KING; i++) {
        rect.x = (3 + (i - 2) % 2) * squareDimension;
        rect.y = (3 + (i - 2) / 2) * squareDimension;
        rect.w = squareDimension;
        rect.h = squareDimension;
        drawPiece(rect, Piece(i + colourConst));
    }
    SDL_RenderPresent(renderer);
}

void ChessGUI::drawPiece(SDL_Rect rect, Piece piece) {
    if (piece) {
        SDL_Texture* texture = pieceTextures[piece];
        if (texture) {
            SDL_RenderCopy(renderer, texture, NULL, &rect);
        }
    }
}

void ChessGUI::quitChessGUI() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int ChessGUI::screenCoordToBoardIndex(int x, int y) {
    SDL_GetWindowSize(window, &width, &height);
    int boardSideLength = std::min(width, height);
    int squareDimension = boardSideLength/8;
    if (x > boardSideLength || y > boardSideLength) {
        return -1;
    }
    int file = x / squareDimension;
    int rank = 7 - (y / squareDimension);
    return (rank * 8) + file;
}

void ChessGUI::loadTexture() {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    SDL_Surface* loadedSurface;
    SDL_Texture* loadedTexture;

    loadedSurface = IMG_Load("images/WhitePawn.png");
    loadedTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    pieceTextures[WHITEPAWN] = loadedTexture;
    SDL_FreeSurface(loadedSurface);

    loadedSurface = IMG_Load("images/WhiteKing.png");
    loadedTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    pieceTextures[WHITEKING] = loadedTexture;
    SDL_FreeSurface(loadedSurface);

    loadedSurface= IMG_Load("images/WhiteQueen.png");
    loadedTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    pieceTextures[WHITEQUEEN] = loadedTexture;
    SDL_FreeSurface(loadedSurface);

    loadedSurface = IMG_Load("images/WhiteBishop.png");
    loadedTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    pieceTextures[WHITEBISHOP] = loadedTexture;
    SDL_FreeSurface(loadedSurface);

    loadedSurface = IMG_Load("images/WhiteKnight.png");
    loadedTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    pieceTextures[WHITEKNIGHT] = loadedTexture;
    SDL_FreeSurface(loadedSurface);

    loadedSurface= IMG_Load("images/WhiteRook.png");
    loadedTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    pieceTextures[WHITEROOK] = loadedTexture;
    SDL_FreeSurface(loadedSurface);

    loadedSurface= IMG_Load("images/BlackPawn.png");
    loadedTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    pieceTextures[BLACKPAWN] = loadedTexture;
    SDL_FreeSurface(loadedSurface);

    loadedSurface = IMG_Load("images/BlackKing.png");
    loadedTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    pieceTextures[BLACKKING] = loadedTexture;
    SDL_FreeSurface(loadedSurface);

    loadedSurface = IMG_Load("images/BlackQueen.png");
    loadedTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    pieceTextures[BLACKQUEEN] = loadedTexture;
    SDL_FreeSurface(loadedSurface);

    loadedSurface= IMG_Load("images/BlackBishop.png");
    loadedTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    pieceTextures[BLACKBISHOP] = loadedTexture;
    SDL_FreeSurface(loadedSurface);

    loadedSurface = IMG_Load("images/BlackKnight.png");
    loadedTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    pieceTextures[BLACKKNIGHT] = loadedTexture;
    SDL_FreeSurface(loadedSurface);

    loadedSurface = IMG_Load("images/BlackRook.png");
    loadedTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    pieceTextures[BLACKROOK] = loadedTexture;
    SDL_FreeSurface(loadedSurface);
}