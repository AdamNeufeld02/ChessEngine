#ifndef SEARCH_H
#define SEARCH_H
#include <chrono>
#include "MoveGenerator.h"
#include "ChessBoard.h"
#include "Types.h"
#include "Evaluation.h"
#include "MovePick.h"
#include "TransposTable.h"
#include "Misc.h"

#define MAXDEPTH 64

struct Stack {
    int ply;
    Move* pv;
    Move killers[2];
};

enum NodeType {
    PV, NonPV
};

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
    int searchRoot(ChessBoard& cb, Stack* ss, int alpha, int beta, int depth);
    int widenSearch(ChessBoard& cb, Stack* ss, int val, int depth);
    template<NodeType type>
    int search(ChessBoard& cb, Stack* ss, int alpha, int beta, int depth);
    template<NodeType type>
    int quiesce(ChessBoard& cb, int alpha, int beta);
    void checkTime();
    void updatePv(Move* pv, Move move, Move* childPv);
}

#endif