#ifndef EVALUATION_H
#define EVALUATION_H
#include "Types.h"

extern int mgVals[8];
extern int psPawn[64];
extern int psKnight[64];
extern int psBishop[64];
extern int psRook[64];
extern int psQueen[64];
extern int psKing[64];

extern int pieceSquareTables[PIECENB][64];


#include "ChessBoard.h"

class ChessBoard;

static const int infinity = 20000;
static const int draw = -1;

namespace Evaluation {
    int evaluate(ChessBoard& cb);
    void init();
}

#endif