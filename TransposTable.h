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
    int eval;
    Move hashMove;
    int depth;
    Type type;

    Entry() {
        key = 0;
        eval = 0;
        depth = 0;
        hashMove = NOMOVE;
        type = Exact;
    }

    Entry(zobristKey _key, int _eval, int _depth, Move _hashMove, Type _type) {
        key = _key;
        eval = _eval;
        depth = _depth;
        hashMove = _hashMove;
        type = _type;
    }
};

class TransposTable {
    public:
    // Default constructor
    TransposTable();
    // Clears all entries in a hash table
    void clear();
    // Adds an entry to the hash table replacing whatever was there previously
    void addEntry(zobristKey key, Move hashMove, int eval, int depth, Type type);
    // Returns true if we can use the evaluation stored in the hash table and false otherwise
    bool probe(zobristKey key, int depth, int beta, int* eval, Move *hashMove);

    Entry* getEntry(zobristKey key) {
        return &table[key % size];
    }

    private:
    // Number of entryies in the hash table
    static const int size = 1000000;
    // The table storing all entries
    Entry table[size];
};

#endif