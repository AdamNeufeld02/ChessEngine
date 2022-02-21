#ifndef TRANSPOSTABLE_H
#define TRANSPOSTABLE_H

#include "Zobrist.h"
#include "Types.h"
#include "ChessBoard.h"


enum Type {
    Exact, Upper, Lower
};

// 8 bytes for the data
// bytes 0 - 2 for hash move
// bytes 2 - 4 for score
// bytes 4 - 6 for eval
// byte 6 for depth
// byte 7 for type
struct Entry {
    zobristKey key;
    uint64_t data;

    Entry() {
        key = 0;
        data = 0;
    }

    Entry(zobristKey _key, uint16_t _score, uint16_t _eval, uint8_t _depth, Move _hashMove, uint8_t _type) {
        key = _key;
        data = (uint64_t) _hashMove;     // 0 - 16;
        data += (uint64_t) _score << 16; // 16 - 32
        data += (uint64_t) _eval << 32; // 32 - 48
        data += (uint64_t) _depth << 48; // 48 - 56
        data += (uint64_t) _type << 56;  // 56 - 64
    }

    Move getMove() {
        return Move(data & 0xffff);
    }

    int getScore() {
        union { uint16_t u; int16_t s; } score = { uint16_t(unsigned(data + 0x8000) >> 16) };
        return score.s;
    }

    int getEval() {
        return int16_t(data >> 32);
    }

    int getDepth() {
        return (data >> 48) & 0xff;
    }

    Type getType() {
        return Type((data >> 56) & 0xff);
    }

    void setEval(uint16_t eval) {
        data ^= (uint64_t)getEval() << 32;
        data |= (uint64_t)eval << 32;
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
    Entry* probe(zobristKey key, bool* hit);

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