#include "Zobrist.h"

zobristKey Zobrist::psq[PIECENB][64];
zobristKey Zobrist::castlingKeys[CASTLING_RIGHT_NB];
zobristKey Zobrist::epSquare[64];
zobristKey Zobrist::colToMove;

void Zobrist::init() {
    colToMove = Misc::genRand64();
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < PIECENB; j++) {
            psq[j][i] = Misc::genRand64();
        }
        epSquare[i] = Misc::genRand64();
    }
    for (int i = 0; i < CASTLING_RIGHT_NB; i++) {
        castlingKeys[i] = Misc::genRand64();
    }
}