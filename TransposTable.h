#ifndef TRANSPOSTABLE_H
#define TRANSPOSTABLE_H

#include "Zobrist.h"
#include "Types.h"
#include "ChessBoard.h"

enum Type {
    Exact, Upper, Lower
};

struct Entry {
    zobristKey key;
    Move hashMove;
    int eval;
    int depth;
    Type type;
};

class TransposTable {
    public:
    TransposTable();
    void addEntry(zobristKey key, Move hashMove, int eval, int depth, Type type);
    bool probe(zobristKey key, int depth, Move *hashMove);

    private:
    static const int size = 1000000;
    Entry table[size];
};

#endif