#include "Evaluation.h"
#include "Threads.h"

// material values, King has no material values as it is assumed it is always on the board
Score pieceVals[7] = {Score(0, 0), Score(92, 153), Score(436, 348), Score(478, 377), Score(586, 650), Score(1160, 1181), Score(0, 0)};

Score passedBonus[8] = {Score(0, 0), Score(11, 9), Score(19, 11), Score(21, 34), Score(36, 60), Score(36, 82), Score(44, 77), Score(0, 0)};
Score connectedBonus[8] = {Score(0, 0), Score(3, -13), Score(19, 4), Score(18, 13), Score(18, 7), Score(48, 9), Score(33, 34), Score(0, 0)};
Score isolated = Score(-10, -2);
Score doubled = Score(14, -13);
Score unsupported = Score(-6, -8);
Score supported = Score(7, 7);

int manhattanDist[64][64];

bitBoard aheadMasks[2][64];
bitBoard neighbourFiles[8];

Score shelterBonus[4][8] = {{Score(-2, 0), Score(30, 0), Score(23, 0), Score(15, 0), Score(0, 0), Score(-5, 0), Score(-9, 0), Score(-15, 0)},
                          {Score(-15, 0), Score(27, 0), Score(18, 0), Score(7, 0), Score(-7, 0), Score(-13, 0), Score(-15, 0), Score(-22, 0)},
                          {Score(-4, 0), Score(22, 0), Score(11, 0), Score(3, 0), Score(-9, 0), Score(-18, 0), Score(-20, 0), Score(-27, 0)},
                          {Score(-13, 0), Score(17, 0), Score(10, 0), Score(-1, 0), Score(-15, 0), Score(-20, 0), Score(-25, 0), Score(-35, 0)}};

Score blockedStorm[8] = {Score(0, 0), Score(35, 0), Score(4, 0), Score(-4, 0), Score(-4, 0), Score(-4, 0), Score(-4, 0), Score(-4, 0)};
Score unblockedStorm[8] = {Score(17, 0), Score(-50, 0), Score(-30, 0), Score(2, 0), Score(8, 0), Score(12, 0), Score(12, 0), Score(12, 0)};
Score openFileBonus[2][2] = {{Score(44, -14), Score(13, 10)},
                             {Score(6, 10), Score(-40, 0), }};


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

Score rookOnOpenFile = Score(41, 2);

Score knightMobility[9] = {Score(3, -18), Score(10, -18), Score(15, 0), Score(21, 13), Score(33, 13), 
                           Score(41, 21), Score(47, 22), Score(47, 24), Score(60, 27)};

Score bishopMobility[14] = {Score(-10, -60), Score(-4, -32), Score(7, -9), Score(17, -5), Score(25, 8), 
                            Score(26, 18), Score(33, 21), Score(36, 22), Score(41, 23), Score(46, 23), 
                            Score(48, 24), Score(55, 26), Score(56, 27), Score(57, 28)};

Score rookMobility[15] = {Score(-33, -64), Score(-21, -32), Score(-22, 2), Score(-17, 13), Score(-15, 25), 
                          Score(-8, 29), Score(-8, 39), Score(0, 39), Score(12, 39), Score(16, 40), 
                          Score(23, 40), Score(48, 42), Score(54, 44), Score(54, 46), Score(54, 48)};

Score queenMobility[28] = {Score(12, 0), Score(6, -3), Score(7, -10), Score(0, -9), Score(10, -22), 
                          Score(10, 3), Score(4, 5), Score(8, 12), Score(9, 26), Score(10, 43), 
                          Score(20, 31), Score(16, 48), Score(22, 50), Score(37, 48), Score(26, 55), 
                          Score(27, 62), Score(42, 64), Score(46, 71), Score(41, 71), Score(54, 63), 
                          Score(53, 68), Score(42, 79), Score(36, 56), Score(28, 43), Score(7, 16), 
                          Score(8, 14), Score(0, -3), Score(3, 6)};

// Piece Square table for pawns oriented to blacks perspective
Score psPawn[64] = {
    Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), 
    Score(108, 196), Score(110, 177), Score(75, 151), Score(89, 126), Score(76, 140), Score(85, 113), Score(52, 142), Score(60, 185), 
    Score(11, 93), Score(-4, 97), Score(20, 67), Score(11, 45), Score(54, 23), Score(69, 27), Score(14, 67), Score(-1, 84), 
    Score(-16, 12), Score(-16, -7), Score(-5, -24), Score(6, -45), Score(1, -50), Score(16, -35), Score(-13, -31), Score(-24, -11), 
    Score(-24, -11), Score(-23, -37), Score(2, -45), Score(-5, -49), Score(2, -51), Score(2, -60), Score(-17, -48), Score(-26, -41), 
    Score(-25, -30), Score(-23, -36), Score(-10, -45), Score(-15, -20), Score(-12, -34), Score(-5, -45), Score(11, -61), Score(-12, -49), 
    Score(-8, -10), Score(11, -31), Score(-5, -23), Score(-5, -13), Score(-6, -11), Score(36, -39), Score(43, -54), Score(11, -44), 
    Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0)
};

// Piece Square table for knights Symmetrical
Score psKnight[64] = {
    Score(-97, -66), Score(-46, -48), Score(-30, -17), Score(-33, -39), Score(-24, -22), Score(-46, -51), Score(-39, -35), Score(-64, -85), 
    Score(-66, -43), Score(-53, -12), Score(32, -11), Score(-9, -1), Score(-5, -4), Score(7, -31), Score(-17, -28), Score(-53, -47), 
    Score(-50, -41), Score(13, -5), Score(6, 21), Score(31, 15), Score(50, 3), Score(55, 9), Score(31, -21), Score(-11, -40), 
    Score(-18, 2), Score(4, 16), Score(4, 34), Score(49, 30), Score(22, 33), Score(65, 20), Score(9, 1), Score(-6, -13), 
    Score(-9, -10), Score(-9, -6), Score(4, 34), Score(16, 40), Score(36, 26), Score(0, 37), Score(18, 9), Score(-6, -13), 
    Score(-29, -27), Score(-12, 8), Score(15, -6), Score(-16, 35), Score(16, 26), Score(18, -3), Score(27, -11), Score(-14, -11), 
    Score(-34, -37), Score(-38, -22), Score(-21, 0), Score(14, -7), Score(14, 0), Score(10, -21), Score(-21, -14), Score(-21, -39), 
    Score(-61, -44), Score(5, -54), Score(-38, -24), Score(-12, -13), Score(-4, 0), Score(-25, 6), Score(3, -37), Score(-55, -43)
};

// Piece Square table for bishops in blacks perspective
Score psBishop[64] = {
    Score(-22, -23), Score(0, -14), Score(-33, -30), Score(-27, -20), Score(-9, 2), Score(-21, -17), Score(-8, -6), Score(-18, -11), 
    Score(-25, -17), Score(-7, -6), Score(-25, -1), Score(-21, -14), Score(11, 8), Score(7, 2), Score(1, -1), Score(-56, -26), 
    Score(-31, 6), Score(8, -6), Score(26, -8), Score(14, 2), Score(19, 0), Score(27, 17), Score(2, 11), Score(-15, 9), 
    Score(-7, 0), Score(8, 7), Score(4, 15), Score(49, 19), Score(16, 18), Score(20, 8), Score(13, 0), Score(-12, 10), 
    Score(2, -12), Score(3, 2), Score(12, 19), Score(25, 22), Score(35, 7), Score(7, 17), Score(2, -5), Score(9, -17), 
    Score(-5, -6), Score(12, 6), Score(15, 27), Score(20, 9), Score(22, 26), Score(33, 1), Score(11, 0), Score(7, -19), 
    Score(2, 2), Score(22, -5), Score(11, 0), Score(24, 1), Score(25, 4), Score(18, -7), Score(49, -13), Score(-9, -24), 
    Score(-21, -11), Score(1, 2), Score(24, -7), Score(-15, 8), Score(-5, 4), Score(25, -7), Score(-29, -22), Score(-33, -18)
};

// Piece Square Table for Rooks oriented in Blacks perspective
Score psRook[64] = {
    Score(9, 18), Score(5, 21), Score(-7, 24), Score(3, 20), Score(9, 15), Score(-6, 16), Score(-4, 8), Score(-5, 10), 
    Score(3, 18), Score(11, 18), Score(29, 18), Score(24, 16), Score(29, 2), Score(34, -1), Score(2, 6), Score(7, 13), 
    Score(-2, 16), Score(3, 16), Score(8, 14), Score(14, 8), Score(-2, 5), Score(16, 1), Score(12, 1), Score(-3, 4), 
    Score(-20, 20), Score(-12, 16), Score(8, 14), Score(17, 6), Score(8, 12), Score(29, 8), Score(-10, -5), Score(-10, 12), 
    Score(-43, 23), Score(-17, 16), Score(-12, 21), Score(-2, 11), Score(6, 4), Score(7, -5), Score(0, -2), Score(-29, 3), 
    Score(-42, 5), Score(-22, 15), Score(-6, -4), Score(-4, 1), Score(-3, 0), Score(19, -15), Score(14, -13), Score(-27, -10), 
    Score(-41, 6), Score(-6, -7), Score(-15, 12), Score(-5, 12), Score(15, -13), Score(40, -19), Score(7, -7), Score(-70, 2), 
    Score(24, -11), Score(8, 8), Score(20, 9), Score(31, -8), Score(31, -12), Score(54, -19), Score(-21, 5), Score(19, -33)
};


// Piece Square Table for Queens oriented in Blacks perspective
Score psQueen[64] = {
    Score(-30, -19), Score(-9, 10), Score(-8, 3), Score(0, 20), Score(10, 9), Score(1, 9), Score(6, 2), Score(19, 7), 
    Score(-32, -28), Score(-66, 24), Score(0, 10), Score(15, 26), Score(-7, 17), Score(6, 12), Score(5, 8), Score(28, 13), 
    Score(-17, 0), Score(-12, 2), Score(5, 4), Score(5, 38), Score(15, 26), Score(29, 21), Score(3, 9), Score(29, 1), 
    Score(-29, 29), Score(-22, 37), Score(-12, 29), Score(-26, 49), Score(12, 36), Score(5, 18), Score(5, 23), Score(14, 23), 
    Score(0, 10), Score(-32, 34), Score(11, 34), Score(12, 50), Score(5, 50), Score(15, 39), Score(10, 38), Score(3, 21), 
    Score(-21, 23), Score(26, -8), Score(18, 21), Score(28, 1), Score(15, 27), Score(29, 32), Score(23, 24), Score(17, 11), 
    Score(-25, -5), Score(3, -7), Score(43, -13), Score(38, -9), Score(47, 4), Score(34, -11), Score(19, -5), Score(20, -6), 
    Score(15, -4), Score(17, -3), Score(23, -4), Score(62, -42), Score(20, 13), Score(3, -2), Score(-5, -7), Score(-35, -28)
};
// Piece Square Table for Kings oriented in Blacks perspective
Score psKing[64] = {
    Score(-31, -55), Score(-41, -53), Score(-37, -37), Score(-54, -48), Score(-49, -19), Score(-42, -28), Score(-33, -20), Score(-29, -48), 
    Score(-23, -25), Score(-33, -2), Score(-36, 0), Score(-43, -3), Score(-49, -1), Score(-29, 37), Score(-36, 14), Score(-34, -19), 
    Score(-23, -13), Score(-28, 16), Score(-26, 18), Score(-48, 10), Score(-36, 20), Score(-22, 60), Score(-12, 43), Score(-28, 3), 
    Score(-34, -32), Score(-33, 25), Score(-34, 19), Score(-43, 30), Score(-38, 33), Score(-27, 38), Score(-23, 30), Score(-33, -6), 
    Score(-39, -43), Score(-27, 0), Score(-31, 27), Score(-58, 28), Score(-47, 31), Score(-29, 24), Score(-33, 0), Score(-43, -40), 
    Score(-25, -34), Score(-24, -1), Score(-18, 18), Score(-39, 28), Score(-39, 23), Score(-14, 15), Score(4, -11), Score(-33, -30), 
    Score(15, -57), Score(25, -32), Score(-2, 1), Score(-55, 8), Score(-36, 11), Score(-21, 2), Score(24, -28), Score(34, -55), 
    Score(-15, -105), Score(14, -81), Score(12, -50), Score(-60, -15), Score(14, -52), Score(-67, -24), Score(11, -67), Score(18, -100) 
};

Score pieceSquareTables[PIECENB][64];

EvalTrace Evaluation::trace;

void Evaluation::init() {
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

    if (tableEntry && key == tableEntry->key) {
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
    shelter += openFileBonus[cb.onOpenFile(col, ksq)][cb.onOpenFile(~col, ksq)];
    return shelter;
}

template<Colour us>
Score Evaluation::evaluateAttacks(ChessBoard& cb) {
    constexpr Direction upRight = ~us == WHITE ? NORTHEAST : SOUTHEAST;
    constexpr Direction upLeft = ~us == WHITE ? NORTHWEST : SOUTHWEST;
    Score sc = Score(0, 0);
    int theirKing = getLSBIndex(cb.pieces(~us, KING));
    // Bonus for attack the enemy king zone
    bitBoard targetKing = genAttacksBB<KING>(theirKing) | squares[theirKing];
    bitBoard occ = cb.pieces();
    bitBoard enemeyPawns = cb.pieces(~us, PAWN);
    bitBoard pawnAtt = (shift<upRight>(enemeyPawns) & ~FileHBB) | (shift<upLeft>(enemeyPawns) & ~FileABB);
    bitBoard open = ~cb.pieces(us) & ~pawnAtt;

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
    }
    // Evaluate Bishop attacks
    bb = cb.pieces(us, BISHOP);
    while (bb) {
        sq = popLSB(bb);
        attacks = genAttacksBB<BISHOP>(sq, occ);
        sc += bishopMobility[countBits(attacks & open)];
        weightedAttacks += countBits(attacks & targetKing) * attackerWeight[BISHOP];

    }
    // Evaluate Rook attacks
    bb = cb.pieces(us, ROOK);
    while (bb) {
        sq = popLSB(bb);
        attacks = genAttacksBB<ROOK>(sq, occ);
        sc += rookMobility[countBits(attacks & open)];

        if (cb.onOpenFile(us, sq)) {
            sc += rookOnOpenFile;
        }

        weightedAttacks += countBits(attacks & targetKing) * attackerWeight[ROOK];
    }
    // Evaluate Queen attacks
    bb = cb.pieces(us, QUEEN);
    while (bb) {
        sq = popLSB(bb);
        attacks = genAttacksBB<QUEEN>(sq, occ);
        sc += queenMobility[countBits(attacks & open)];
        weightedAttacks += countBits(attacks & targetKing) * attackerWeight[QUEEN];
    }
    sc += safetyTable[weightedAttacks];
    return sc;
}

int Evaluation::relativeRank(Colour col, int sq) {
    int rank = sq / 8;
    return col == WHITE? rank : 7 - rank;
}

int Evaluation::distToSide(int file) {
    return std::min(file, 7 - file);
}