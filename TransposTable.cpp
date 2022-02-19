#include "TransposTable.h"

TransposTable::TransposTable() {
    clear();
}

void TransposTable::clear() {
    for (int i = 0; i < size; i++) {
        table[i] = Entry(0, 0, NoValue, 0, NOMOVE, Exact);
    }
}

void TransposTable::addEntry(zobristKey key, Move hashMove, int score, int eval, int depth, Type type) {
    Entry* tte = getEntry(key);

    // Overwrite entries that search to a deeper depth. With slight priority to new entries. Also include exact entries always
    if (depth >= tte->getDepth() - 1 || type == Exact) {
        Move move = hashMove;
        int nEval = eval;
        if (key == tte->key) {
            if (move == NOMOVE) {
                move = tte->getMove();
            }

            if (nEval == NoValue) {
                nEval = tte->getEval();
            }
        }
        table[key % size] = Entry(key, score, nEval, depth, move, type);
        return;
    // If we do not have an evaluation stored try and store one
    } else if (key == tte->key) {
        if(tte->getEval() == NoValue) {
            tte->setEval(eval);
        }
    }
}

Entry* TransposTable::probe(zobristKey key, bool* hit) {
    Entry* entry = getEntry(key);

    if (entry->key == key) {
        *hit = true;
    } else {
        *hit = false;
    }

    return entry;
}