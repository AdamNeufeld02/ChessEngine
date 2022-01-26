#ifndef MOVEPICK_H
#define MOVEPICK_H
#include "Types.h"

class MovePick {
    public:
    MovePick(Move* allMoves, int initLength);
    Move getNext();
    private:
    Move* moves;
    int length;
    void score();
};

#endif