#include "Evaluation.h"
#include "Threads.h"

// material values, King has no material values as it is assumed it is always on the board
Score pieceVals[7] = {Score(0, 0), Score(100, 100), Score(300, 300), Score(330, 330), Score(500, 500), Score(900, 900), Score(0, 0)};

Score passedBonus[8] = {Score(0, 0), Score(7, 13), Score(12, 19), Score(18, 27), Score(24, 37), Score(31, 47), Score(42, 55), Score(0, 0)};
Score connectedBonus[8] = {Score(0, 0), Score(7, 7), Score(9, 9), Score(11, 11), Score(15, 15), Score(22, 22), Score(28, 28), Score(0, 0)};
Score isolated = Score(-5, -12);
Score doubled = Score(-5, -11);
Score unsupported = Score(-9, -13);
Score supported = Score(7, 7);

int manhattanDist[64][64];

bitBoard aheadMasks[2][64];
bitBoard neighbourFiles[8];

Score shelterBonus[4][8] = {{Score(-2, 0), Score(30, 0), Score(27, 0), Score(15, 0), Score(0, 0), Score(-5, 0), Score(-9, 0), Score(-15, 0)},
                          {Score(-15, 0), Score(27, 0), Score(18, 0), Score(7, 0), Score(-7, 0), Score(-13, 0), Score(-15, 0), Score(-22, 0)},
                          {Score(-4, 0), Score(22, 0), Score(11, 0), Score(3, 0), Score(-9, 0), Score(-18, 0), Score(-20, 0), Score(-27, 0)},
                          {Score(-13, 0), Score(17, 0), Score(10, 0), Score(-1, 0), Score(-15, 0), Score(-20, 0), Score(-25, 0), Score(-35, 0)}};

Score blockedStorm[8] = {Score(0, 0), Score(35, 0), Score(4, 0), Score(-4, 0), Score(-4, 0), Score(-4, 0), Score(-4, 0), Score(-4, 0)};
Score unblockedStorm[8] = {Score(17, 0), Score(-50, 0), Score(-30, 0), Score(2, 0), Score(8, 0), Score(12, 0), Score(12, 0), Score(12, 0)};
Score openFileBonus[2][2] = {{Score(22, -4), Score(13, -3)},
                             {Score(0, 4), Score(-12, 10)}};

Score safetyTable[100] = {
    Score(0, 0),  Score(0, 0),   Score(1, 1),   Score(2, 2),   Score(3, 3),
    Score(5, 5),  Score(7, 7),   Score(9, 9),   Score(12, 12), Score(15, 15),
    Score(18, 18),  Score(22, 22),  Score(26, 26),  Score(30, 30),  Score(35, 35),
    Score(39, 39),  Score(44, 44),  Score(50, 50),  Score(56, 56),  Score(62, 62),
    Score(68, 68),  Score(75, 75),  Score(82, 82),  Score(85, 85),  Score(89, 89),
    Score(97, 97), Score(105, 105), Score(113, 113), Score(122, 122), Score(131,131),
    Score(140, 140), Score(150, 150), Score(169, 169), Score(180, 180), Score(191, 191),
    Score(202, 202), Score(213, 213), Score(225, 225), Score(237, 237), Score(248, 248),
    Score(260, 260), Score(272, 272), Score(283, 283), Score(295, 295), Score(307, 307),
    Score(319, 319), Score(330, 330), Score(342, 342), Score(354, 354), Score(366, 366),
    Score(377, 377), Score(389, 389), Score(401, 401), Score(412, 412), Score(424, 424),
    Score(436, 436), Score(448, 448), Score(459, 459), Score(471, 471), Score(483, 483),
    Score(494, 494), Score(500, 500), Score(500, 500), Score(500, 500), Score(500, 500),
    Score(500, 500), Score(500, 500), Score(500, 500), Score(500, 500), Score(500, 500),
    Score(500, 500), Score(500, 500), Score(500, 500), Score(500, 500), Score(500, 500),
    Score(500, 500), Score(500, 500), Score(500, 500), Score(500, 500), Score(500, 500),
    Score(500, 500), Score(500, 500), Score(500, 500), Score(500, 500), Score(500, 500),
    Score(500, 500), Score(500, 500), Score(500, 500), Score(500, 500), Score(500, 500),
    Score(500, 500), Score(500, 500), Score(500, 500), Score(500, 500), Score(500, 500),
    Score(500, 500), Score(500, 500), Score(500, 500), Score(500, 500), Score(500, 500)
};

int attackerWeight[8] = {0, 1, 2, 2, 3, 6, 0};

Score knightMobility[9] = {Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0),
                           Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0)};

Score bishopMobility[14] = {Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0),
                            Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0),
                            Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0)};

Score rookMobility[15] = {Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0),
                          Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0),
                          Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0)};

Score queenMobility[28] = {Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0),
                           Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0),
                           Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0),
                           Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0),
                           Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0),
                           Score(0, 0), Score(0, 0), Score(0, 0)};

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

bool Evaluation::doTrace = false;
EvalTrace Evaluation::trace;

void Evaluation::init() {
    doTrace = false;
    for (int i = 0; i < 64; i++) {
        int rank = i / 8;
        int file = i % 8;
        int whiteIdx = (7 - rank) * 8 + file;
        // init psq tables
        pieceSquareTables[BLACKPAWN][i] = psPawn[i];
        pieceSquareTables[BLACKKNIGHT][i] = psKnight[i];
        pieceSquareTables[BLACKBISHOP][i] = psBishop[i];
        pieceSquareTables[BLACKROOK][i] = psRook[i];
        pieceSquareTables[BLACKQUEEN][i] = psQueen[i];
        pieceSquareTables[BLACKKING][i] = psKing[i];
        pieceSquareTables[WHITEPAWN][i] = psPawn[whiteIdx];
        pieceSquareTables[WHITEKNIGHT][i] = psKnight[whiteIdx];
        pieceSquareTables[WHITEBISHOP][i] = psBishop[whiteIdx];
        pieceSquareTables[WHITEROOK][i] = psRook[whiteIdx];
        pieceSquareTables[WHITEQUEEN][i] = psQueen[whiteIdx];
        pieceSquareTables[WHITEKING][i] = psKing[whiteIdx];

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
}

int Evaluation::evaluate(ChessBoard& cb) {
    zobristKey key = cb.pawnKey();
    pawnEntry* tableEntry = NULL; 
    if (cb.thisThread) {
        tableEntry = cb.thisThread->pTT.probe(key);
    }
    
    Score pawnStructScore;

    if (tableEntry && !doTrace && key == tableEntry->key) {
        pawnStructScore = tableEntry->score;
    } else {
        pawnStructScore = evaluatePawnStructure<WHITE>(cb) + evaluateKingShelter<WHITE>(cb) 
                          - evaluatePawnStructure<BLACK>(cb) - evaluateKingShelter<BLACK>(cb);

        if (tableEntry) {
            tableEntry->key = key;
            tableEntry->score = pawnStructScore;
        }
    }

    Score total = cb.getPSQT(WHITE) + evaluateAttacks<WHITE>(cb) + cb.getMaterial(WHITE)
                 - cb.getPSQT(BLACK) - evaluateAttacks<BLACK>(cb) - cb.getMaterial(BLACK);

    total += pawnStructScore;

    double phase = 4 * countBits(cb.pieces(QUEEN)) +
                2 * countBits(cb.pieces(ROOK)) +
                1 * countBits(cb.pieces(BISHOP)) +
                1 * countBits(cb.pieces(KNIGHT));
    
    int ret = total.mg * phase / 24.0 + total.eg *(1 - phase / 24.0);
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
            score += supported * countBits(support);

            if (doTrace) {
                trace.connected[col][relativeRank(col, idx)]++;
                trace.supported[col] += countBits(support);
            }

        // A pawn with no pawns in adjecent files is considered isolated and will be penalized
        } else if (!neighbours) {
            score += isolated;

            if (doTrace)
                trace.isolated[col]++;
        }
        // Passed pawns recieve a bonus
        if (!ahead && !opposing) {
            score += passedBonus[relativeRank(col, idx)];

            if (doTrace)
                trace.passed[col][relativeRank(col, idx)]++;
        // Doubled pawns are penalized
        } else if (ahead) {
            score += doubled;

            if (doTrace)
                trace.doubled[col]++;
        }
        if (!support) {
            score += unsupported;

            if (doTrace)
                trace.unsupported[col]++;
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

    Score shelter = Score(0, 0);
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

        if (doTrace)
            trace.shelter[col][sideDist][ourRank]++;


        // Evaluate Storming Pawns
        immediate = theirPawns & aheadMasks[col][8 * kingRank + file];
        // Look at their closest pawn in front of our king
        int theirRank = immediate ? relativeRank(col, getFrontMost(~col, immediate)) : 0;
        int rankDist = theirRank ? theirRank - relativeRank(col, ksq) : 0;
        if (ourRank == theirRank - 1) {
            shelter += blockedStorm[rankDist];
            if (doTrace) 
                trace.blockedStorm[col][rankDist]++;
        } else {
            shelter += unblockedStorm[rankDist];
            if (doTrace)
                trace.unblockedStorm[col][rankDist]++;
        }
    }
    shelter += openFileBonus[cb.onOpenFile(col, ksq)][cb.onOpenFile(~col, ksq)];

    if (doTrace) 
        trace.openFile[col][cb.onOpenFile(col, ksq)][cb.onOpenFile(~col, ksq)]++;
    return shelter;
}

template<Colour us>
Score Evaluation::evaluateAttacks(ChessBoard& cb) {
    Score sc = Score(0, 0);
    int theirKing = getLSBIndex(cb.pieces(~us, KING));
    // Bonus for attack the enemy king zone
    bitBoard targetKing = genAttacksBB<KING>(theirKing) | squares[theirKing];
    bitBoard occ = cb.pieces();
    bitBoard open = ~cb.pieces(us);

    bitBoard bb = cb.pieces(us, KNIGHT);
    bitBoard attacks = 0;
    int sq;
    int weightedAttacks = 0;
    // Evaluate Knight attacks
    while (bb) {
        sq = popLSB(bb);
        attacks = genAttacksBB<KNIGHT>(sq);
        sc += knightMobility[countBits(attacks & open)];
        weightedAttacks += countBits(attacks & targetKing) * attackerWeight[KNIGHT];

        if (doTrace) {
            trace.knightMob[us][countBits(attacks & open)]++;
        }
    }
    // Evaluate Bishop attacks
    bb = cb.pieces(us, BISHOP);
    while (bb) {
        sq = popLSB(bb);
        attacks = genAttacksBB<BISHOP>(sq, occ);
        sc += bishopMobility[countBits(attacks & open)];
        weightedAttacks += countBits(attacks & targetKing) * attackerWeight[BISHOP];

        if (doTrace) {
            trace.bishopMob[us][countBits(attacks & open)]++;
        }
    }
    // Evaluate Rook attacks
    bb = cb.pieces(us, ROOK);
    while (bb) {
        sq = popLSB(bb);
        attacks = genAttacksBB<ROOK>(sq, occ);
        sc += rookMobility[countBits(attacks & open)];
        weightedAttacks += countBits(attacks & targetKing) * attackerWeight[ROOK];

        if (doTrace) {
            trace.rookMob[us][countBits(attacks & open)]++;
        }
    }
    // Evaluate Queen attacks
    bb = cb.pieces(us, QUEEN);
    while (bb) {
        sq = popLSB(bb);
        attacks = genAttacksBB<QUEEN>(sq, occ);
        sc += queenMobility[countBits(attacks & open)];
        weightedAttacks += countBits(attacks & targetKing) * attackerWeight[QUEEN];

        if (doTrace) {
            trace.queenMob[us][countBits(attacks & open)]++;
        }
    }
    sc += safetyTable[weightedAttacks];
    if (doTrace) {
        trace.safety[us][weightedAttacks]++;
    }
    return sc;
}

int Evaluation::relativeRank(Colour col, int sq) {
    int rank = sq / 8;
    return col == WHITE? rank : 7 - rank;
}

int Evaluation::distToSide(int file) {
    return std::min(file, 7 - file);
}