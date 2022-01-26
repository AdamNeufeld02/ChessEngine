#ifndef SEARCH_H
#define SEARCH_H
#include "MoveGenerator.h"
#include "ChessBoard.h"
#include "Types.h"
#include "Evaluation.h"
#include "MovePick.h"

static int nodesSearched;

namespace Search {
    Move searchStart(ChessBoard& cb, int depth);
    int search(ChessBoard& cb, int alpha, int beta, int depth);
    int quiesce(ChessBoard& cb, int alpha, int beta);
}

#endif