#include "Evaluation.h"

pawnTT Evaluation::pTT;

// Midgame material values, King has no material values as it is assumed it is always on the board
int mgVals[8] = {0, 100, 300, 330, 500, 900, 0};

Score passedBonus[8] = {Score(0, 0), Score(7, 13), Score(12, 19), Score(18, 27), Score(24, 37), Score(31, 47), Score(42, 55), Score(0, 0)};
int connectedBonus[8] = {0, 7, 9, 11, 15, 22, 28, 0};
Score isolated = Score(-5, -12);
Score doubled = Score(-5, -11);
Score unsupported = Score(-9, -22);

int manhattanDist[64][64];

bitBoard aheadMasks[2][64];
bitBoard neighbourFiles[8];

int shelterBonus[4][8] = {{-2, 30, 27, 15, 0, -5, -9, -15},
                          {-15, 27, 18, 7, -7, -13, -15, -22},
                          {-4, 22, 11, 3, -9, -18, -20, -27},
                          {-13, 17, 10, -1, -15, -20, -25, -35}};

int blockedStorm[8] = {0, 35, 4, -4, -4, -4, -4, -4};
int unblockedStorm[8] = {17, -50, -30, 2, 8, 12, 12, 12};
Score openFileBonus[2][2] = {{Score(22, -4), Score(13, -3)},
                             {Score(0, 4), Score(-12, 10)}};

int safetyTable[100] = {
    0,  0,   1,   2,   3,   5,   7,   9,  12,  15,
    18,  22,  26,  30,  35,  39,  44,  50,  56,  62,
    68,  75,  82,  85,  89,  97, 105, 113, 122, 131,
    140, 150, 169, 180, 191, 202, 213, 225, 237, 248,
    260, 272, 283, 295, 307, 319, 330, 342, 354, 366,
    377, 389, 401, 412, 424, 436, 448, 459, 471, 483,
    494, 500, 500, 500, 500, 500, 500, 500, 500, 500,
    500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
    500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
    500, 500, 500, 500, 500, 500, 500, 500, 500, 500
};

int attackerWeight[8] = {0, 1, 2, 2, 3, 6, 0};

// Piece Square table for pawns oriented to blacks perspective
Score psPawn[64] = {
    Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
    Score(50, 50), Score(50, 50), Score(50, 50), Score(50, 50), Score(50, 50), Score(50, 50), Score(50, 50), Score(50, 50),
    Score(10, 10), Score(10, 10), Score(20, 20), Score(30, 30), Score(30, 30),  Score(20, 20), Score(10, 10), Score(10, 10),
    Score(5, 5), Score(5, 5), Score(10, 10), Score(25, 25), Score(25, 25), Score(10, 10), Score(5, 5), Score(5, 5),
    Score(0, 0), Score(0, 0), Score(0, 0), Score(20, 20), Score(20, 20), Score(0, 0), Score(0, 0), Score(0, 0),
    Score(5, 5), Score(-5, -5), Score(-10, -10), Score(0, 0), Score(0, 0), Score(-10, -10), Score(-5, -5), Score(5, 5),
    Score(5, 5), Score(10, 10), Score(10, 10), Score(-20, -20),Score(-20, -20), Score(10, 10), Score(10, 10), Score(5, 5),
    Score(0, 0), Score(0, 0),  Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0)
};

// Piece Square table for knights Symmetrical
Score psKnight[64] = {
    Score(-50, -50), Score(-40, -40), Score(-30, -30), Score(-30, -30), Score(-30, -30), Score(-30, -30), Score(-40, -40), Score(-50, -50),
    Score(-40, -40), Score(-20, -20),  Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(-20, -20), Score(-40, -40),
    Score(-30, -30), Score(0, 0), Score(10, 10), Score(15, 15), Score(15, 15), Score(10, 10), Score(0, 0), Score(-30, -30),
    Score(-30, -30), Score(5, 5), Score(15, 15), Score(20, 20), Score(20, 20), Score(15, 15), Score(5, 5), Score(-30, -30),
    Score(-30, -30), Score(0, 0), Score(15, 15), Score(20, 20), Score(20, 20), Score(15, 15), Score(0, 0), Score(-30, -30),
    Score(-30, -30), Score(5, 5), Score(10, 10), Score(15, 15), Score(15, 15), Score(10, 10), Score(5, 5), Score(-30, 30),
    Score(-40, -40), Score(-20, -20), Score(0, 0), Score(5, -5), Score(5, 5), Score(0, 0), Score(-20, -20), Score(-40, -40),
    Score(-50, -50), Score(-40, -40), Score(-30, -30), Score(-30, -30), Score(-30, -30), Score(-30, -30), Score(-40, -40), Score(-50, -50)
};

// Piece Square table for bishops in blacks perspective
Score psBishop[64] = {
    Score(-20, -20), Score(-10, -10), Score(-10, -10), Score(-10, -10), Score(-10, -10), Score(-10, -10), Score(-10, -10), Score(-20, -10),
    Score(-10, -10), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(-10, -10),
    Score(-10, -10), Score(0, 0), Score(5, 5), Score(10, 10), Score(10, 10), Score(5, 5), Score(0, 0), Score(-10, -10),
    Score(-10, -10), Score(5, 5), Score(5, 5), Score(10, 10), Score(10, 10), Score(5, 5), Score(5, 5), Score(-10, -10),
    Score(-10, -10), Score(0, 0), Score(10, 10), Score(10, 10), Score(10, 10), Score(10, 10),  Score(0, 0),  Score(-10, -10),
    Score(-10, -10), Score(10, 10), Score(10, 10), Score(10, 10), Score(10, 10), Score(10, 10), Score(10, 10), Score(-10, -10),
    Score(-10, -10), Score(5, 5), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(5, 5), Score(-10, -10),
    Score(-20, -20), Score(-10, -10), Score(-10, -10), Score(-10, -10), Score(-10, -10), Score(-10, -10), Score(-10, -10), Score(-20, -20)
};

// Piece Square Table for Rooks oriented in Blacks perspective
Score psRook[64] = {
    Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0),
    Score(5, 5), Score(10, 10), Score(10, 10), Score(10, 10), Score(10, 10), Score(10, 10), Score(10, 10), Score(5, 5),
    Score(-5, -5), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(-5, -5),
    Score(-5, -5), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(-5, -5),
    Score(-5, -5), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(-5, -5),
    Score(-5, -5), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(-5, -5),
    Score(-5, -5), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(-5, -5),
    Score(0, 0), Score(0, 0), Score(0, 0), Score(5, 5), Score(5, 5), Score(0, 0), Score(0, 0), Score(0, 0)
};

// Piece Square Table for Queens oriented in Blacks perspective
Score psQueen[64] = {
    Score(-20, -20), Score(-10, -10), Score(-10, -10), Score(-5, -5), Score(-5, -5), Score(-10, -10), Score(-10, -10), Score(-20, -20),
    Score(-10, -10), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(-10, -10),
    Score(-10, -10), Score(0, 0), Score(5, 5), Score(5, 5), Score(5, 5), Score(5, 5), Score(0, 0), Score(-10, -10),
    Score(-5, -5), Score(0, 0), Score(5, 5), Score(5, 5), Score(5, 5), Score(5, 5), Score(0, 0), Score(-5, -5),
    Score(-5, -5), Score(0, 0), Score(5, 5), Score(5, 5), Score(5, 5), Score(5, 5), Score(0, 0), Score(0, 0),
    Score(-10, 10), Score(0, 0), Score(5, 5), Score(5, 5), Score(5, 5), Score(5, 5), Score(5, 5), Score(-10, -10),
    Score(-10, -10), Score(0, 0), Score(5, 5), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(-10, -10),
    Score(-20, -20), Score(-10, -10), Score(-10, -10), Score(-5, -5), Score(-5, -5), Score(-10, -10), Score(-10, -10), Score(-20, -20)
}; 

// Piece Square Table for Kings oriented in Blacks perspective
Score psKing[64] = {
    Score(-30, -50), Score(-40, -40), Score(-40, -30), Score(-50, -20), Score(-50, -20), Score(-40, -30), Score(-40, -40), Score(-30, -50),
    Score(-30, -30), Score(-40, -20), Score(-40, -10), Score(-50, 0), Score(-50, 0), Score(-40, -10), Score(-40, -20), Score(-30, -30),
    Score(-30, -30), Score(-40, -10), Score(-40, 20), Score(-50, 30), Score(-50, 30), Score(-40, 20), Score(-40, -10), Score(-30, -30),
    Score(-30, -30), Score(-40, -10), Score(-40, 30), Score(-50, 40), Score(-50, 40), Score(-40, 30), Score(-40, -10), Score(-30, -30),
    Score(-30, -30), Score(-40, -10), Score(-40, 30), Score(-50, 40), Score(-50, 40), Score(-40, 30), Score(-40, -10), Score(-30, -30),
    Score(-30, -30), Score(-40, -10), Score(-40, 20), Score(-50, 30), Score(-50, 30), Score(-40, 20), Score(-40, -10), Score(-30, -30),
    Score(20, -30), Score(20, -30), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(20, -30), Score(20, -30),
    Score(20, -50), Score(30, -30), Score(10, -30), Score(0, -30), Score(0, -30), Score(10, -30), Score(30, -30), Score(20, -50)
};

Score pieceSquareTables[PIECENB][64];

void Evaluation::init() {
    for (int i = 0; i < 64; i++) {
        // init psq tables
        pieceSquareTables[BLACKPAWN][i] = psPawn[i];
        pieceSquareTables[BLACKKNIGHT][i] = psKnight[i];
        pieceSquareTables[BLACKBISHOP][i] = psBishop[i];
        pieceSquareTables[BLACKROOK][i] = psRook[i];
        pieceSquareTables[BLACKQUEEN][i] = psQueen[i];
        pieceSquareTables[BLACKKING][i] = psKing[i];
        pieceSquareTables[WHITEPAWN][i] = psPawn[63 - i];
        pieceSquareTables[WHITEKNIGHT][i] = psKnight[63 - i];
        pieceSquareTables[WHITEBISHOP][i] = psBishop[63 - i];
        pieceSquareTables[WHITEROOK][i] = psRook[63 - i];
        pieceSquareTables[WHITEQUEEN][i] = psQueen[63 - i];
        pieceSquareTables[WHITEKING][i] = psKing[63 - i];


        int rank = i / 8;
        int file = i % 8;
        // init pawn structure masks
        bitBoard wAhead = 0;
        bitBoard bAhead = 0;
        
        for (int j = rank + 1; j < 8; j++) {
            setBit(wAhead, (j * 8 + file));
        }
        for (int j = rank - 1; j >= 0; j--) {
            setBit(bAhead, (j * 8 + file));
        }
        aheadMasks[WHITE][i] = wAhead;
        aheadMasks[BLACK][i] = bAhead;

        for (int j = 0; j < 64; j++) {
            manhattanDist[i][j] = std::abs((j / 8) - rank) + std::abs((j % 8) - file);
        }

    }   

    for (int i = 0; i < 8; i++) {
        bitBoard adjFiles = 0;
        if (i > 0) {
            for (int j = 0; j < 8; j++) {
                setBit(adjFiles, j * 8 + i - 1);
            }
        }
        if (i < 7) {
            for (int j = 0; j < 8; j++) {
                setBit(adjFiles, j * 8 + i + 1);
            }
        }
        neighbourFiles[i] = adjFiles;
    }
    pTT.set(120000);
    pTT.clear();
}

int Evaluation::evaluate(ChessBoard& cb) {
    zobristKey key = cb.pawnKey();
    pawnEntry* tableEntry = pTT.probe(key);
    Score pawnStructScore;

    if (key == tableEntry->key) {
        pawnStructScore = tableEntry->score;
    } else {
        pawnStructScore = evaluatePawnStructure<WHITE>(cb) + evaluateKingShelter<WHITE>(cb) 
                          - evaluatePawnStructure<BLACK>(cb) - evaluateKingShelter<BLACK>(cb);
        tableEntry->key = key;
        tableEntry->score = pawnStructScore;
    }

    Score total = cb.getPSQT(WHITE) + evaluateKingZone<WHITE>(cb)
                 - cb.getPSQT(BLACK) - evaluateKingZone<BLACK>(cb);
    
    int whiteMat = cb.getMaterial(WHITE);
    int blackMat = cb.getMaterial(BLACK);
    
    int totalMat = whiteMat + blackMat;
    total += pawnStructScore;
    int ret = total.mg *((double)totalMat / 7920) + total.eg *((double)7920 - totalMat) / 7920 + whiteMat - blackMat;
    return cb.colourToMove() == WHITE? ret : -ret;
}

template<Colour col>
Score Evaluation::evaluatePawnStructure(ChessBoard& cb) {
    Direction up = col == WHITE ? NORTH : SOUTH;
    bitBoard ourPawns = cb.pieces(col, PAWN);
    bitBoard theirPawns = cb.pieces(~col, PAWN);
    bitBoard b = ourPawns;

    Score score = Score(0, 0);

    while (b) {
        int idx = popLSB(b);
        int file = idx % 8;
        int rank = idx / 8;
        bitBoard neighbours = ourPawns & neighbourFiles[file];
        bitBoard support = neighbours & rankBB[rank - (up / 8)];
        bitBoard adj = neighbours & rankBB[rank];
        bitBoard ahead = ourPawns & aheadMasks[col][idx];
        bitBoard opposing = theirPawns & (aheadMasks[col][idx] | 
                            (neighbourFiles[file] & aheadMasks[col][idx + 1] & aheadMasks[col][idx - 1]));
        // A pawn supported by others recieves a bonus
        if (support || adj) {
            score += connectedBonus[relativeRank(col, idx)];
            score += 7 * countBits(support);
        // A pawn with no pawns in adjecent files is considered isolated and will be penalized
        } else if (!neighbours) {
            score += isolated;
        }
        // Passed pawns recieve a bonus
        if (!ahead && !opposing) {
            score += passedBonus[relativeRank(col, idx)];
        // Doubled pawns are penalized
        } else if (ahead) {
            score += doubled;
        }
        if (!support) {
            score += unsupported;
        }

    }
    return score;
}

template<Colour col>
Score Evaluation::evaluateKingShelter(ChessBoard& cb) {
    int ksq = getLSBIndex(cb.pieces(col, KING));
    bitBoard ourPawns = cb.pieces(col, PAWN);
    bitBoard theirPawns = cb.pieces(~col, PAWN);
    int kingRank = ksq / 8;
    int kingFile = ksq % 8;

    int shelter = 0;
    int numfiles = 0;

    // Evaluation of Pawn Storm and Pawn Shelter
    for (int file = std::max(kingFile - 1, 0); file <= std::min(kingFile + 1, 7); file++) {
        numfiles++;
        // Evaluate Pawn Shelter
        bitBoard immediate = ourPawns & aheadMasks[col][8 * kingRank + file];
        // Look at first pawn ahead of king on this file
        int ourRank = immediate ? relativeRank(col, getFrontMost(~col, immediate)) : 0;
        int sideDist = distToSide(file);
        shelter += shelterBonus[sideDist][ourRank];


        // Evaluate Storming Pawns
        immediate = theirPawns & aheadMasks[col][8 * kingRank + file];
        // Look at their closest pawn in front of our king
        int theirRank = immediate ? relativeRank(col, getFrontMost(~col, immediate)) : 0;
        int rankDist = theirRank ? theirRank - relativeRank(col, ksq) : 0;
        if (ourRank == theirRank - 1) {
            shelter += blockedStorm[rankDist];
        } else {
            shelter += unblockedStorm[rankDist];
        }
    }
    Score ret = Score(shelter/(numfiles - 1), 0);
    ret += openFileBonus[cb.onOpenFile(col, ksq)][cb.onOpenFile(~col, ksq)];
    return ret;
}

template<Colour col>
Score Evaluation::evaluateKingZone(ChessBoard& cb) {
    int ksq = getLSBIndex(cb.pieces(col, KING));
    bitBoard kingZone = genAttacksBB<KING>(ksq) | squares[ksq];
    bitBoard occ = cb.pieces() & ~squares[ksq];

    bitBoard bb = cb.pieces(~col, KNIGHT);
    int weightedAttacks = getWeightedAttacks<KNIGHT>(kingZone, bb, occ);

    bb = cb.pieces(~col, BISHOP);
    weightedAttacks += getWeightedAttacks<BISHOP>(kingZone, bb, occ);

    bb = cb.pieces(~col, ROOK);
    weightedAttacks += getWeightedAttacks<ROOK>(kingZone, bb, occ);

    bb = cb.pieces(~col, QUEEN);
    weightedAttacks += getWeightedAttacks<QUEEN>(kingZone, bb, occ);
    

    return Score(-safetyTable[weightedAttacks], -safetyTable[weightedAttacks]);
}

template<PieceType pt>
int Evaluation::getWeightedAttacks(bitBoard kingZone, bitBoard attackers, bitBoard occ) {
    int sq;
    int weightedAttacks = 0;
    bitBoard attacks;
    while (attackers) {
        sq = popLSB(attackers);
        attacks = genAttacksBB<pt>(sq, occ);
        weightedAttacks += countBits(attacks & kingZone) * attackerWeight[pt];
    }
    return weightedAttacks;
}

int Evaluation::relativeRank(Colour col, int sq) {
    int rank = sq / 8;
    return col == WHITE? rank : 7 - rank;
}

int Evaluation::distToSide(int file) {
    return std::min(file, 7 - file);
}