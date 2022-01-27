#include "Evaluation.h"

// Midgame material values, King has no material values as it is assumed it is always on the board
int mgVals[8] = {0, 100, 300, 330, 500, 900, 0};

// Piece Square table for pawns oriented to blacks perspective
int psPawn[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
};

// Piece Square table for knights Symmetrical
int psKnight[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
};

// Piece Square table for bishops in blacks perspective
int psBishop[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
};

// Piece Square Table for Rooks oriented in Blacks perspective
int psRook[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  0,  5,  5,  0,  0,  0
};

// Piece Square Table for Queens oriented in Blacks perspective
int psQueen[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
    -5,  0,  5,  5,  5,  5,  0, 0,
    -10,  0,  5,  5,  5,  5,  5,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

// Piece Square Table for Kings oriented in Blacks perspective
int psKing[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20
};

int pieceSquareTables[PIECENB][64];

void Evaluation::init() {
    for (int i = 0; i < 64; i++) {
        pieceSquareTables[BLACKPAWN][i] = psPawn[i];
        pieceSquareTables[BLACKKNIGHT][i] = psKnight[i];
        pieceSquareTables[BLACKBISHOP][i] = psBishop[i];
        pieceSquareTables[BLACKROOK][i] = psRook[i];
        pieceSquareTables[BLACKQUEEN][i] = psQueen[i];
        pieceSquareTables[BLACKKING][i] = psKing[i];
    }
    for (int i = 0; i < 64; i++) {
        pieceSquareTables[WHITEPAWN][i] = psPawn[63 - i];
        pieceSquareTables[WHITEKNIGHT][i] = psKnight[63 - i];
        pieceSquareTables[WHITEBISHOP][i] = psBishop[63 - i];
        pieceSquareTables[WHITEROOK][i] = psRook[63 - i];
        pieceSquareTables[WHITEQUEEN][i] = psQueen[63 - i];
        pieceSquareTables[WHITEKING][i] = psKing[63 - i];
    }
}

int Evaluation::evaluate(ChessBoard& cb) {
    return cb.colourToMove() == WHITE? cb.getMaterial(WHITE) - cb.getMaterial(BLACK) : cb.getMaterial(BLACK) - cb.getMaterial(WHITE);
}