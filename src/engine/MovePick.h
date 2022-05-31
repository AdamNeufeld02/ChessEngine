#ifndef MOVEPICK_H
#define MOVEPICK_H
#include "Types.h"
#include "Evaluation.h"
#include "ChessBoard.h"

// TODO: Change to use staged system instead. Generate moves only as we need them.
// Gen captures first and only if no cuttoff occurs move on to quiet generation.

// TODO: Score captures with a mix of SEE and capture history. Keep quiet scoring the same.


enum Stage {
    TTMove, GoodCaptures, EvenCaptures, Refutations, AllQuiets, LosingCaptures
};

// Used for alpha beta move ordering. Implemented with a priority queue/heap as if a move causes a beta cuttoff we dont 
// have to sort the rest of the moves and can just return.

class MovePick {
    public:
    // Constructs a scored move priority queue
    MovePick(ScoredMove* allMoves, int initLength, Move pref, Move prev, Move* killers, ChessBoard& cb);
    // Returns the most promising next move if no moves left return NOMOVE
    Move getNext();
    private:
    ScoredMove* moves;
    Move counter;
    int length;
    // scores all moves by MVV LVA
    void score(ChessBoard& cb, Move pref, Move* killers);
    void buildHeap();
    void heapifyDown(int start);
    int getLeftChild(int parent);
    int getRightChild(int parent);
    void swap(int ind1, int ind2);
};

#endif