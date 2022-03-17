#ifndef SEARCH_H
#define SEARCH_H
#include <chrono>
#include <algorithm>
#include "MoveGenerator.h"
#include "ChessBoard.h"
#include "Types.h"
#include "Evaluation.h"
#include "MovePick.h"
#include "TransposTable.h"
#include "Misc.h"

struct Stack {
    int ply;
    Move* pv;
    Move killers[2];
    Move current;
    bool didNull;
};

enum NodeType {
    PV, RootPV, NonPV
};

namespace Search {
    static const int aspiration = 50;
    extern int reductions[MAXMOVES];
    extern TransposTable tTable;
    void init();
    void updatePv(Move* pv, Move move, Move* childPv);
}

#endif