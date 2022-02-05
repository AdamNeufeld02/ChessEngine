#ifndef SEARCH_H
#define SEARCH_H
#include <chrono>
#include "MoveGenerator.h"
#include "ChessBoard.h"
#include "Types.h"
#include "Evaluation.h"
#include "MovePick.h"
#include "TransposTable.h"

namespace Search {
    extern int nodesSearched;
    extern int currDepth;
    extern bool abort;
    extern std::chrono::steady_clock::time_point start;
    extern int alloted;
    extern TransposTable tTable;
    void init();
    Move searchStart(ChessBoard& cb, int time);
    int search(ChessBoard& cb, int alpha, int beta, int depth);
    int quiesce(ChessBoard& cb, int alpha, int beta);
    void checkTime();
}

#endif