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
    static const int aspiration = 50;
    extern int nodesSearched;
    extern int currDepth;
    extern Move bestFound;
    extern bool abort;
    extern std::chrono::steady_clock::time_point start;
    extern int alloted;
    extern TransposTable tTable;
    void init();
    Move searchStart(ChessBoard& cb, int time);
    int searchRoot(ChessBoard& cb, int alpha, int beta, int depth);
    int widenSearch(ChessBoard& cb, int val, int depth);
    int search(ChessBoard& cb, int alpha, int beta, int depth);
    int quiesce(ChessBoard& cb, int alpha, int beta);
    void checkTime();
}

#endif