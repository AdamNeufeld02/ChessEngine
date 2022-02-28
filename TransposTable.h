#ifndef TRANSPOSTABLE_H
#define TRANSPOSTABLE_H

#include "Zobrist.h"
#include "Types.h"
#include "ChessBoard.h"


enum Type {
    Exact, Upper, Lower
};

inline uint64_t makeData(uint16_t _score, uint16_t _eval, uint8_t _depth, Move _hashMove, uint8_t _type) {
    uint64_t data = (uint64_t) _hashMove;     // 0 - 16;
    data += (uint64_t) _score << 16; // 16 - 32
    data += (uint64_t) _eval << 32; // 32 - 48
    data += (uint64_t) _depth << 48; // 48 - 56
    data += (uint64_t) _type << 56;  // 56 - 64
    return data;
}

inline Move unpackMove(uint64_t data) {
    return Move(data & 0xffff);
}

inline int unpackScore(uint64_t data) {
    union { uint16_t u; int16_t s; } score = { uint16_t(unsigned(data + 0x8000) >> 16) };
    return score.s;
}

inline int unpackEval(uint64_t data) {
    return int16_t(data >> 32);
}

inline int unpackDepth(uint64_t data) {
   return (data >> 48) & 0xff;
}

inline Type unpackType(uint64_t data) {
    return Type((data >> 56) & 0xff);
}

// 8 bytes for the data
// bytes 0 - 2 for hash move
// bytes 2 - 4 for score
// bytes 4 - 6 for eval
// byte 6 for depth
// byte 7 for type
struct Entry {
    zobristKey keyXData;
    uint64_t data;

    Entry() {
        keyXData = 0;
        data = 0;
    }

    void save(zobristKey key, uint16_t score, uint16_t eval, uint8_t depth, Move hashMove, uint8_t type) {
        if (depth >= unpackDepth(data) || type == Exact) {
            uint64_t data_ = makeData(score, eval, depth, hashMove, type);
            data = data_;
            keyXData = key ^ data_;
        }
    }
};

class TransposTable {
    public:
    // Default constructor
    TransposTable();
    // Clears all entries in a hash table
    void clear();
    // Adds an entry to the hash table replacing whatever was there previously
    void addEntry(zobristKey key, Move hashMove, int score, int eval, int depth, Type type);
    // Returns true if we can use the evaluation stored in the hash table and false otherwise
    uint64_t probe(zobristKey key, bool* hit);

    Entry* getEntry(zobristKey key) {
        return &table[key % size];
    }

    private:
    // Number of entries in the hash table
    static const int size = 1000000;
    // The table storing all entries
    Entry table[size];
};
#endif