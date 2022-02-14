#include "GameState.h"

const static std::string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

GameState::GameState() {
    states = std::unique_ptr<std::deque<StateInfo>>(new std::deque<StateInfo>(1));
    states->emplace_back();
    chessBoard = new ChessBoard(startingFen, states->back());
    gui = new ChessGUI(); 
    gameState = PLAYING_GAME;
    player1.colour = WHITE;
    player2.colour = BLACK;
    player1.isHuman = false;
    player2.isHuman = true;
    chessBoard->printBoard(chessBoard->pieces());
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
}

void GameState::gameLoop() {
    struct Player playerToMove = getPlayerToMove();
    Move move;
    while (gameState == PLAYING_GAME) {
        if(playerToMove.isHuman) {
            move = getMoveFromUser();
        } else {
            move = getMoveFromComp();
        }
        states->emplace_back();
        chessBoard->doMove(move, states->back());
        std::cout << "Hash: " << chessBoard->key() <<std::endl;
        std::cout << "--------------------" << std::endl;
        std::cout << chessBoard->getPSQT(WHITE).mg << std::endl;
        std::cout << chessBoard->getPSQT(BLACK).mg << std::endl;
        playerToMove = getPlayerToMove();
    }
}

Move GameState::getMoveFromComp() {
    ScoredMove moves[MAXMOVES];
    ScoredMove* end = MoveGenerator::generateMoves(*chessBoard, moves, false);
    int length = end - moves;
    int movMat[64];
    resetMoveMatrix(movMat);
    gui->drawBoard(chessBoard, -1, movMat);
    if (length == 0) {
        gameState = GAME_OVER;
        std::cout << "COMPUTER LOST";
        return NOMOVE;
    }
    Move move = Search::searchStart(*chessBoard, 3);
    return move;
}

Move GameState::getMoveFromUser() {
    Move move = NOMOVE;
    ScoredMove moves[MAXMOVES];
    ScoredMove* end = MoveGenerator::generateMoves(*chessBoard, moves, false);
    SDL_Event ev;
    Piece promPiece;
    int x, y;
    int selectedIndex = -1;
    int movMat[64];
    resetMoveMatrix(movMat);
    gui->drawBoard(chessBoard, selectedIndex, movMat);
    if (end == moves) {
        std::cout << "Player Lost";
        gameState = GAME_OVER;
        return NOMOVE;
    }
    while (move == NOMOVE) {
        while(SDL_PollEvent(&ev) != 0) {
            if (ev.type == SDL_QUIT) {
                gameState = QUIT;
                return NOMOVE;
            } else if (ev.type == SDL_MOUSEBUTTONDOWN) {
                SDL_GetMouseState(&x, &y);
                selectedIndex = gui->screenCoordToBoardIndex(x, y);
                makeMoveMatrix(moves, end, selectedIndex, movMat);
            } else if (ev.type == SDL_MOUSEBUTTONUP) {
                SDL_GetMouseState(&x, &y);
                move = findMove(moves, end, selectedIndex, gui->screenCoordToBoardIndex(x, y));
                if (getFlag(move) == PROMOTION) {
                    promPiece = getPromotionFromUser(chessBoard->colourToMove());
                    move = makeMove(getFrom(move), getTo(move), typeOf(promPiece));
                }
                resetMoveMatrix(movMat);
                selectedIndex = -1;
            }
            gui->drawBoard(chessBoard, selectedIndex, movMat);
        }
    }
    return move;
}

Piece GameState::getPromotionFromUser(Colour colour) {
    Piece promPiece = EMPTY;
    int x, y;
    int index;
    SDL_Event ev;
    gui->drawPromSelection(colour); 
    while (promPiece == EMPTY) {
        while(SDL_PollEvent(&ev) != 0) {
            if (ev.type == SDL_QUIT) {
                gameState = QUIT;
                return EMPTY;
            } else if (ev.type == SDL_MOUSEBUTTONDOWN) {
                SDL_GetMouseState(&x, &y);
                index = gui->screenCoordToBoardIndex(x, y);
                if (index == 27) {
                    return Piece(ROOK + (colour << 3));
                } else if (index == 28) {
                    return Piece(QUEEN + (colour << 3));
                } else if (index == 35) {
                    return Piece(KNIGHT + (colour << 3));
                } else if (index == 36) {
                    return Piece(BISHOP + (colour << 3));
                }
            } if (ev.type == SDL_MOUSEMOTION) {
                gui->drawPromSelection(colour); 
            }
        }
    }
    return promPiece;
}

Move GameState::findMove(ScoredMove* begin, ScoredMove* end, int from, int to) {
    int size = end - begin;
    for (int i = 0; i < size; i++) {
        Move move = begin[i].move;
        if (getFrom(move) == from && getTo(move) == to) {
            return move;
        }
    }
    // Return empty move
    return NOMOVE;
}

void GameState::resetMoveMatrix(int* movMat) {
    for (int i = 0; i < 64; i++) movMat[i] = 0;
}

void GameState::makeMoveMatrix(ScoredMove* begin, ScoredMove* end, int index, int* movMat) {
    int size = end - begin;
    // init movMat to 0
    resetMoveMatrix(movMat);
    // Find moves and set squares in matrix
    for (int i = 0; i < size; i++) {
        if (getFrom(begin[i].move) == index) {
            movMat[getTo(begin[i].move)] += 1;
        }
    }
}

struct Player GameState::getPlayerToMove() {
    if (player1.colour == chessBoard->colourToMove()) {
        return player1;
    } else {
        return player2;
    }
}

void GameState::gameOver() {
    while (1);
    gameState = QUIT;
}