#ifndef ZOBRIST_H
#define ZOBRIST_H
#include "Types.h"
#include "Misc.h"
#include <cstdint>

typedef uint64_t zobristKey;

namespace Zobrist {
    extern zobristKey psq[PIECENB][64];
    extern zobristKey castlingKeys[CASTLING_RIGHT_NB];
    extern zobristKey epSquare[64];
    extern zobristKey colToMove;

    void init();
}

#endif