#ifndef EVALUATION_H
#define EVALUATION_H
#include "Types.h"
#include "BitBoards.h"

extern int mgVals[8];
extern Score psPawn[64];
extern Score psKnight[64];
extern Score psBishop[64];
extern Score psRook[64];
extern Score psQueen[64];
extern Score psKing[64];

extern Score pieceSquareTables[PIECENB][64];
extern bitBoard aheadMasks[2][64];
extern bitBoard neighbourFiles[8];

// A bonus for passed pawns on a given rank
extern Score passedBonus[8];
// A bonus for connected pawns on a given rank
extern int connectedBonus[8];
// A penalty for pawns with no friendly pawns on neighbour file
extern Score isolated;
// A penalty for pawns which have double up on a single file
extern Score doubled;
// A penalty for pawns with no supporting pawn
extern Score unsupported;

extern int manhattanDist[64][64];

// Bonus for pawn shelters based on distance to the edge and relative rank
extern int shelterBonus[4][8];
// A penalty for king standing on open or semi open file
extern Score openFileBonus[2][2];
// A penalty/bonus for the rank of a blocked pawn storm
extern int blockedStorm[8];
// A penalty/bonus for the rank of an unblocked pawn storm
extern int unblockedStorm[8];

// Penalties for the number of attacks on the king zone
extern int safetyTable[100];
// The weight of each attack on the king zone by piece
extern int attackerWeight[8];

#include "ChessBoard.h"

class ChessBoard;

static const int infinity = 20000;
static const int draw = -1;

// A simple way to evaluate a chess board. Material scores are kept within the board and updated at every do and undo move.

namespace Evaluation {
    int evaluate(ChessBoard& cb);
    template<Colour col>
    Score evaluatePawnStructure(ChessBoard& cb);
    template<Colour col>
    Score evaluateKingShelter(ChessBoard& cb);
    template<Colour col>
    Score evaluateKingZone(ChessBoard& cb);
    template<PieceType pt>
    int getWeightedAttacks(bitBoard kingZone, bitBoard attackers, bitBoard occ);
    int relativeRank(Colour col, int sq);
    int distToSide(int file);
    void init();
}

#endif