#ifndef SEARCH_H
#define SEARCH_H
#include "MoveGenerator.h"
#include "ChessBoard.h"
#include "Types.h"
#include "Evaluation.h"

namespace Search {
    Move searchStart(ChessBoard& cb, int depth);
    int search(ChessBoard& cb, int depth);
}

#endif