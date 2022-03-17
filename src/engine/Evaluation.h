#ifndef EVALUATION_H
#define EVALUATION_H
#include "Types.h"
#include "BitBoards.h"
#include "Zobrist.h"

extern Score pieceVals[8];
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
extern Score connectedBonus[8];
// A penalty for pawns with no friendly pawns on neighbour file
extern Score isolated;
// A penalty for pawns which have double up on a single file
extern Score doubled;
// A penalty for pawns with no supporting pawn
extern Score unsupported;

extern int manhattanDist[64][64];

// Bonus for pawn shelters based on distance to the edge and relative rank
extern Score shelterBonus[4][8];
// A penalty for king standing on open or semi open file
extern Score openFileBonus[2][2];
// A penalty/bonus for the rank of a blocked pawn storm
extern Score blockedStorm[8];
// A penalty/bonus for the rank of an unblocked pawn storm
extern Score unblockedStorm[8];

// Penalties for the number of attacks on the king zone
extern int safetyTable[100];
// The weight of each attack on the king zone by piece
extern int attackerWeight[8];

extern Score knightMobility[9];
extern Score bishopMobility[14];
extern Score rookMobility[15];
extern Score queenMobility[28];

#include "ChessBoard.h"

class ChessBoard;

static const int draw = -1;

struct EvalTrace {
    // Pawn Eval Trace
    int connected[COLOURNB][8];
    int passed[COLOURNB][8];
    int isolated[COLOURNB];
    int doubled[COLOURNB];
    int unsupported[COLOURNB];
    // King Shelter Trace
    int shelter[COLOURNB][4][8];
    int blockedStorm[COLOURNB][8];
    int unblockedStorm[COLOURNB][8];
    int openFile[COLOURNB][2][2];
    // King Safety Trace
    int safety[COLOURNB][100];
    // Mobility Trace
    int knightMob[COLOURNB][9];
    int bishopMob[COLOURNB][14];
    int rookMob[COLOURNB][15];
    int queenMob[COLOURNB][28];
};

struct pawnEntry {
    zobristKey key;
    Score score;
};

class pawnTT {
    public:
    void set(size_t size_) {
        size = size_;
        table = (pawnEntry* )malloc(size * sizeof(pawnEntry));
    }

    void clear() {
        for (unsigned int i = 0; i < size; i++) {
            table[i].key = 0;
            table[i].score = Score(0, 0);
        }
    }
    pawnEntry* probe(zobristKey key) {
        return &table[key % size];
    }

    private:
    size_t size;
    pawnEntry* table;
};

// A simple way to evaluate a chess board. Material scores are kept within the board and updated at every do and undo move.

namespace Evaluation {
    int evaluate(ChessBoard& cb);

    template<Colour col>
    Score evaluatePawnStructure(ChessBoard& cb);

    template<Colour col>
    Score evaluateKingShelter(ChessBoard& cb);

    template<Colour col>
    Score evaluateAttacks(ChessBoard& cb);
    
    int relativeRank(Colour us, int sq);
    int distToSide(int file);
    void init();

    extern EvalTrace trace;
    extern bool doTrace;
}

#endif