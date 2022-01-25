#include "Evaluation.h"

// Midgame material values, King has no material values as it is assumed it is always on the board
int mgVals[8] = {0, 100, 300, 330, 500, 900, 0};

int Evaluation::evaluate(ChessBoard& cb) {
    return cb.colourToMove() == WHITE? cb.getMaterial(WHITE) - cb.getMaterial(BLACK) : cb.getMaterial(BLACK) - cb.getMaterial(WHITE);
}