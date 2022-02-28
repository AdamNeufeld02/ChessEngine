#include "TransposTable.h"

TransposTable::TransposTable() {
    clear();
}

void TransposTable::clear() {
    for (int i = 0; i < size; i++) {
        table[i].keyXData = 0;
        table[i].data = 0;
    }
}

void TransposTable::addEntry(zobristKey key, Move hashMove, int score, int eval, int depth, Type type) {
    Entry* tte = getEntry(key);
    tte->save(key, score, eval, depth, hashMove, type);
}

uint64_t TransposTable::probe(zobristKey key, bool* hit) {
    Entry* entry = getEntry(key);
    uint64_t data = entry->keyXData ^ key;
    *hit = data == entry->data;
    return data;
}