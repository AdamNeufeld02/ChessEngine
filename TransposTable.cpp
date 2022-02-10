#include "TransposTable.h"

TransposTable::TransposTable() {
    clear();
}

void TransposTable::clear() {
    for (int i = 0; i < size; i++) {
        table[i].key = 0;
        table[i].hashMove = NOMOVE;
        table[i].eval = 0;
        table[i].depth = 0;
        table[i].type = Exact;
    }
}

void TransposTable::addEntry(zobristKey key, Move hashMove, int eval, int depth, Type type) {
    table[key % size] = Entry(key, eval, depth, hashMove, type);
}

bool TransposTable::probe(zobristKey key, int depth, int beta, int* eval, Move* hashMove) {
    Entry* entry = getEntry(key);
    // If the key is not in the table we must continue searching
    if (entry->key != key) return false;
    *hashMove = entry->hashMove;
    // If the depth stored is less then search depth we must continue searching
    if (depth > entry->depth) return false;
    *eval = entry->eval;
    // If the lower estimate does not cause a beta cuttoff we must continue searching
    if (entry->eval >= beta) {
        return entry->type == Lower;
    } else {
        return entry->type == Upper || entry->type == Exact;
    }
}