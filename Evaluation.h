#ifndef EVALUATION_H
#define EVALUATION_H

extern int mgVals[8];

#include "Types.h"
#include "ChessBoard.h"

class ChessBoard;

static const int infinity = 20000;

namespace Evaluation {
    int evaluate(ChessBoard& cb);
}

#endif