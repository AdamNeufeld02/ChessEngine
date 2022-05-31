#include "Tune.h"

void Tuner::runTuner() {
    std::cout << "Starting Tuner" << std::endl;
    // params array stores the difference in the starting value param and the tuned value
    double params[NPARAMS][PHASENB] = {0};
    double oldParams[NPARAMS][PHASENB] = {0};
    setParams(oldParams);
    TuningEntry* entries = (TuningEntry*)malloc(NPOSITIONS * sizeof(TuningEntry));
    std::cout << "Initializing entries" << std::endl;
    initEntries(entries);
    
    double k = minimizeK(entries);
    double adaGrad[NPARAMS][PHASENB] = {0};
    double error = 0;

    printf("Minimized K at: %f\n", k);

    std::cout << "SError: " << staticEvalError(entries, k) << " TEerror: " << tunedEvalError(entries, params, k) << std::endl;

    for (int i = 0; i < MAXEPOCHS; i++) {
        double gradient[NPARAMS][PHASENB] = {0};
        computeGradients(entries, params, gradient, k);

        for (int j = 0; j < NPARAMS; j++) {
            // Update the adaGrad vector which is the sum of squared gradients
            adaGrad[j][mg] += pow(2 * k * gradient[j][mg]/ NPOSITIONS, 2);
            adaGrad[j][eg] += pow(2 * k * gradient[j][eg]/ NPOSITIONS, 2);

            // update the params
            params[j][mg] += (2 * k / NPOSITIONS) * gradient[j][mg] * LEARNINGRATE / sqrt(1e-8 + adaGrad[j][mg]);
            params[j][eg] += (2 * k / NPOSITIONS) * gradient[j][eg] * LEARNINGRATE / sqrt(1e-8 + adaGrad[j][eg]);
        }

        // This updates the internal evaluation stored in each entry
        error = tunedEvalError(entries, params, k);
        printf("Epoch: %d Error: %f\n", i, error);

        if (i % UPDATEON == 0) {
            writeParams(params, oldParams);
        }
    }
}

void Tuner::writeParams(double params[NPARAMS][PHASENB], double oldParams[NPARAMS][PHASENB]) {
    std::ofstream f(RESULTFILE);
    dumpParams(params, oldParams, f);
    f.close();
}

void Tuner::dumpParams(double params[NPARAMS][PHASENB], double oldParams[NPARAMS][PHASENB], std::ofstream& f) {
    int idx = 0;
    int mgVal;
    int egVal;
    // We write the parameters to a file in such a way that they can simply be copied and pasted into the new code with minimal formatting.
    if (TUNEMATERIAL) {
        f << "pieceVals[7] = {Score(0, 0), ";
        for (int i = PAWN; i < KING; i++) {
            mgVal =  params[idx][mg] + oldParams[idx][mg];
            egVal =  params[idx][eg] + oldParams[idx][eg]; 
            f << "Score(" << mgVal << ", " << egVal << "), ";
            idx++;
        }
        f << "Score(0, 0)};\n";
    }

    if (TUNEPSQT) {
        f << "psPawn[64] = {\n";
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                mgVal =  params[idx][mg] + oldParams[idx][mg];
                egVal =  params[idx][eg] + oldParams[idx][eg];
                f << "Score(" << mgVal << ", " << egVal << "), ";
                idx++;
            }
            f << "\n";
        }
        f << "};\n";

        f << "psKnight[64] = {\n";
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                mgVal =  params[idx][mg] + oldParams[idx][mg];
                egVal =  params[idx][eg] + oldParams[idx][eg];
                f << "Score(" << mgVal << ", " << egVal << "), ";
                idx++;
            }
            f << "\n";
        }
        f << "};\n";

        f << "psBishop[64] = {\n";
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                mgVal =  params[idx][mg] + oldParams[idx][mg];
                egVal =  params[idx][eg] + oldParams[idx][eg];
                f << "Score(" << mgVal << ", " << egVal << "), ";
                idx++;
            }
            f << "\n";
        }
        f << "};\n";

        f << "psRook[64] = {\n";
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                mgVal =  params[idx][mg] + oldParams[idx][mg];
                egVal =  params[idx][eg] + oldParams[idx][eg];
                f << "Score(" << mgVal << ", " << egVal << "), ";
                idx++;
            }
            f << "\n";
        }
        f << "};\n";

        f << "psQueen[64] = {\n";
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                mgVal =  params[idx][mg] + oldParams[idx][mg];
                egVal =  params[idx][eg] + oldParams[idx][eg];
                f << "Score(" << mgVal << ", " << egVal << "), ";
                idx++;
            }
            f << "\n";
        }
        f << "};\n";

        f << "psKing[64] = {\n";
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                mgVal =  params[idx][mg] + oldParams[idx][mg];
                egVal =  params[idx][eg] + oldParams[idx][eg];
                f << "Score(" << mgVal << ", " << egVal << "), ";
                idx++;
            }
            f << "\n";
        }
        f << "};\n";
    }

    if (TUNEPAWNSTRUCT) {
        f << "connectedBonus[8] = {";
        for (int i = 0; i < 8; i++) {
            mgVal =  params[idx][mg] + oldParams[idx][mg];
            egVal =  params[idx][eg] + oldParams[idx][eg];
            f << "Score(" << mgVal << ", " << egVal << "), ";
            idx++;
        }
        f <<"};\n";
        f << "passedBonus[8] = {";
        for (int i = 0; i < 8; i++) {
            mgVal =  params[idx][mg] + oldParams[idx][mg];
            egVal =  params[idx][eg] + oldParams[idx][eg];
            f << "Score(" << mgVal << ", " << egVal << "), ";
            idx++;
        }
        f <<"};\n";

        f << "isolated = ";
        mgVal =  params[idx][mg] + oldParams[idx][mg];
        egVal =  params[idx][eg] + oldParams[idx][eg];
        f << "Score(" << mgVal << ", " << egVal << ");\n";
        idx++;

        f << "doubled = ";
        mgVal =  params[idx][mg] + oldParams[idx][mg];
        egVal =  params[idx][eg] + oldParams[idx][eg];
        f << "Score(" << mgVal << ", " << egVal << ");\n";
        idx++;

        f << "unsupported = ";
        mgVal =  params[idx][mg] + oldParams[idx][mg];
        egVal =  params[idx][eg] + oldParams[idx][eg];
        f << "Score(" << mgVal << ", " << egVal << ");\n";
        idx++;

        f << "supported = ";
        mgVal =  params[idx][mg] + oldParams[idx][mg];
        egVal =  params[idx][eg] + oldParams[idx][eg];
        f << "Score(" << mgVal << ", " << egVal << ");\n";
        idx++;

        f << "shelterBonus[4][8] = {{";
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 8; k++) {
                mgVal =  params[idx][mg] + oldParams[idx][mg];
                egVal =  params[idx][eg] + oldParams[idx][eg];
                f << "Score(" << mgVal << ", " << egVal << "), ";
                idx++;
            }
            f << "},\n";
        }
        f << "};\n";

        f << "blockedStorm[8] = {";
        for (int k = 0; k < 8; k++) {
            mgVal =  params[idx][mg] + oldParams[idx][mg];
            egVal =  params[idx][eg] + oldParams[idx][eg];
            f << "Score(" << mgVal << ", " << egVal << "), ";
            idx++;
        }
        f << "};\n";

        f << "unblockedStorm[8] = {";
        for (int k = 0; k < 8; k++) {
            mgVal =  params[idx][mg] + oldParams[idx][mg];
            egVal =  params[idx][eg] + oldParams[idx][eg];
            f << "Score(" << mgVal << ", " << egVal << "), ";
            idx++;
        }
        f << "};\n";

        f << "openFileBonus[j][k] = {{";
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                mgVal =  params[idx][mg] + oldParams[idx][mg];
                egVal =  params[idx][eg] + oldParams[idx][eg];
                f << "Score(" << mgVal << ", " << egVal << "), ";
                idx++;
            }
            f << "},\n";
        }
        f << "};\n";
    }

    if (TUNEKINGSAFETY) {
        f << "safetyTable[100] = {\n";
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 5; j++) {
                mgVal =  params[idx][mg] + oldParams[idx][mg];
                egVal =  params[idx][eg] + oldParams[idx][eg];
                f << "Score(" << mgVal << ", " << egVal << "), ";
                idx++;
            }
             f << "\n";
        }
        f << "};\n";
    }

    if (TUNEMOBILITY) {
        f << "rookOnOpenFile = ";
        mgVal =  params[idx][mg] + oldParams[idx][mg];
        egVal =  params[idx][eg] + oldParams[idx][eg];
        f << "Score(" << mgVal << ", " << egVal << ")\n";
        idx++;

        f << "knightMobility[9] = {";
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 5; j++) {
                if ((i * 5 + j) >= 9) break;
                mgVal =  params[idx][mg] + oldParams[idx][mg];
                egVal =  params[idx][eg] + oldParams[idx][eg];
                f << "Score(" << mgVal << ", " << egVal << "), ";
                idx++;
            }
            f << "\n";
        }
        f << "};\n";

        f << "bishopMobility[14] = {";
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 5; j++) {
                if ((i * 5 + j) >= 14) break;
                mgVal =  params[idx][mg] + oldParams[idx][mg];
                egVal =  params[idx][eg] + oldParams[idx][eg];
                f << "Score(" << mgVal << ", " << egVal << "), ";
                idx++;
            }
            f << "\n";
        }
        f << "};\n";

        f << "rookMobility[15] = {";
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 5; j++) {
                mgVal =  params[idx][mg] + oldParams[idx][mg];
                egVal =  params[idx][eg] + oldParams[idx][eg];
                f << "Score(" << mgVal << ", " << egVal << "), ";
                idx++;
            }
            f << "\n";
        }
        f << "};\n";

        f << "queenMobility[28] = {";
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 5; j++) {
                if ((i * 5 + j) >= 28) break;
                mgVal =  params[idx][mg] + oldParams[idx][mg];
                egVal =  params[idx][eg] + oldParams[idx][eg];
                f << "Score(" << mgVal << ", " << egVal << "), ";
                idx++;
            }
            f << "\n";
        }
        f << "};\n";
    }
}

void Tuner::setParams(double params[NPARAMS][PHASENB]) {
    int idx = 0;
    if (TUNEMATERIAL) {
        for (int i = PAWN; i < KING; i++) {
            params[idx][mg] = pieceVals[i].mg;
            params[idx][eg] = pieceVals[i].eg;
            idx++;
        }
    }

    if (TUNEPSQT) {
        for (int i = PAWN; i <= KING; i++) {
            for (int j = 0; j < 64; j++) {
                params[idx][mg] = pieceSquareTables[makePiece(BLACK, PieceType(i))][j].mg;
                params[idx][eg] = pieceSquareTables[makePiece(BLACK, PieceType(i))][j].eg;
                idx++;
            }
        }
    }

    if (TUNEPAWNSTRUCT) {
        for (int j = 0; j < 8; j++) {
            params[idx][mg] = connectedBonus[j].mg;
            params[idx][eg] = connectedBonus[j].eg;
            idx++;
        }
        for (int j = 0; j < 8; j++) {
            params[idx][mg] = passedBonus[j].mg;
            params[idx][eg] = passedBonus[j].eg;
            idx++;
        }
        params[idx][mg] = isolated.mg;
        params[idx][eg] = isolated.eg;
        idx++;

        params[idx][mg] = doubled.mg;
        params[idx][eg] = doubled.eg;
        idx++;

        params[idx][mg] = unsupported.mg;
        params[idx][eg] = unsupported.eg;
        idx++;

        params[idx][mg] = supported.mg;
        params[idx][eg] = supported.eg;
        idx++;

        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 8; k++) {
                params[idx][mg] = shelterBonus[j][k].mg;
                params[idx][eg] = shelterBonus[j][k].eg;
                idx++;
            }
        }

        for (int k = 0; k < 8; k++) {
            params[idx][mg] = blockedStorm[k].mg;
            params[idx][eg] =blockedStorm[k].eg;
            idx++;
        }

        for (int k = 0; k < 8; k++) {
            params[idx][mg] = unblockedStorm[k].mg;
            params[idx][eg] = unblockedStorm[k].eg;
            idx++;
        }

        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                params[idx][mg] = openFileBonus[j][k].mg;
                params[idx][eg] = openFileBonus[j][k].eg;
                idx++;
            }
        }
    }

    if (TUNEKINGSAFETY) {
        for (int i = 0; i < 100; i++) {
            params[idx][mg] = safetyTable[i].mg;
            params[idx][eg] = safetyTable[i].eg;
            idx++;
        }
    }

    if (TUNEMOBILITY) {
        params[idx][mg] = rookOnOpenFile.mg;
        params[idx][eg] = rookOnOpenFile.eg;
        idx++;

        for (int i = 0; i < 9; i++) {
            params[idx][mg] = knightMobility[i].mg;
            params[idx][eg] = knightMobility[i].eg;
            idx++;
        }
        for (int i = 0; i < 14; i++) {
            params[idx][mg] = bishopMobility[i].mg;
            params[idx][eg] = bishopMobility[i].eg;
            idx++;
        }
        for (int i = 0; i < 15; i++) {
            params[idx][mg] = rookMobility[i].mg;
            params[idx][eg] = rookMobility[i].eg;
            idx++;
        }
        for (int i = 0; i < 28; i++) {
            params[idx][mg] = queenMobility[i].mg;
            params[idx][eg] = queenMobility[i].eg;
            idx++;
        }
    }
}

template<Colour col>
void Evaluation::traceEval(ChessBoard& cb) {
    Direction up = col == WHITE ? NORTH : SOUTH;
    bitBoard ourPawns = cb.pieces(col, PAWN);
    bitBoard theirPawns = cb.pieces(~col, PAWN);
    bitBoard b = ourPawns;

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
        // Count supported Pawns
        if (support || adj) {
            trace.connected[col][relativeRank(col, idx)]++;
            trace.supported[col] += countBits(support);
        // Count isolated pawns
        } else if (!neighbours) {
            trace.isolated[col]++;
        }
        // Count Passed pawns
        if (!ahead && !opposing) {
            trace.passed[col][relativeRank(col, idx)]++;
        // Count Doubled pawns
        } else if (ahead) {
            trace.doubled[col]++;
        }
        if (!support) {
            trace.unsupported[col]++;
        }

    }

    int ksq = getLSBIndex(cb.pieces(col, KING));
    int kingRank = ksq / 8;
    int kingFile = ksq % 8;
    int numfiles = 0;

    // Trace Pawn Storm and Pawn Shelter
    for (int file = std::max(kingFile - 1, 0); file <= std::min(kingFile + 1, 7); file++) {
        numfiles++;
        // Evaluate Pawn Shelter
        bitBoard immediate = ourPawns & aheadMasks[col][8 * kingRank + file];
        // Look at first pawn ahead of king on this file
        int ourRank = immediate ? relativeRank(col, getFrontMost(~col, immediate)) : 0;
        int sideDist = distToSide(file);

        trace.shelter[col][sideDist][ourRank]++;


        // Evaluate Storming Pawns
        immediate = theirPawns & aheadMasks[col][8 * kingRank + file];
        // Look at their closest pawn in front of our king
        int theirRank = immediate ? relativeRank(col, getFrontMost(~col, immediate)) : 0;
        int rankDist = theirRank ? theirRank - relativeRank(col, ksq) : 0;
        if (ourRank == theirRank - 1) {
            trace.blockedStorm[col][rankDist]++;
        } else {
            trace.unblockedStorm[col][rankDist]++;
        }
    }
    trace.openFile[col][cb.onOpenFile(col, ksq)][cb.onOpenFile(~col, ksq)]++;

    constexpr Direction upRight = ~col == WHITE ? NORTHEAST : SOUTHEAST;
    constexpr Direction upLeft = ~col == WHITE ? NORTHWEST : SOUTHWEST;
    int theirKing = getLSBIndex(cb.pieces(~col, KING));
    // Bonus for attack the enemy king zone
    bitBoard targetKing = genAttacksBB<KING>(theirKing) | squares[theirKing];
    bitBoard occ = cb.pieces();
    bitBoard pawnAtt = (shift<upRight>(theirPawns) & ~FileHBB) | (shift<upLeft>(theirPawns) & ~FileABB);
    bitBoard open = ~cb.pieces(col) & ~pawnAtt;

    bitBoard bb = cb.pieces(col, KNIGHT);
    bitBoard attacks = 0;
    int sq;
    int weightedAttacks = 0;
    // Evaluate Knight attacks
    while (bb) {
        sq = popLSB(bb);
        attacks = genAttacksBB<KNIGHT>(sq);
        weightedAttacks += countBits(attacks & targetKing) * attackerWeight[KNIGHT];
        trace.knightMob[col][countBits(attacks & open)]++;
    }
    // Evaluate Bishop attacks
    bb = cb.pieces(col, BISHOP);
    while (bb) {
        sq = popLSB(bb);
        attacks = genAttacksBB<BISHOP>(sq, occ);
        weightedAttacks += countBits(attacks & targetKing) * attackerWeight[BISHOP];
        trace.bishopMob[col][countBits(attacks & open)]++;
    }
    // Evaluate Rook attacks
    bb = cb.pieces(col, ROOK);
    while (bb) {
        sq = popLSB(bb);
        attacks = genAttacksBB<ROOK>(sq, occ);

        weightedAttacks += countBits(attacks & targetKing) * attackerWeight[ROOK];
        trace.rookMob[col][countBits(attacks & open)]++;
        if (cb.onOpenFile(col, sq)) {
            trace.rookOnOpenFile[col]++;
        }
    }
    // Evaluate Queen attacks
    bb = cb.pieces(col, QUEEN);
    while (bb) {
        sq = popLSB(bb);
        attacks = genAttacksBB<QUEEN>(sq, occ);
        weightedAttacks += countBits(attacks & targetKing) * attackerWeight[QUEEN];
        trace.queenMob[col][countBits(attacks & open)]++;
    }
    trace.safety[col][weightedAttacks]++;
}

void Tuner::initEntries(TuningEntry* entries) {
    char line[256];
    FILE* f = fopen(TUNINGFILE, "r");
    ChessBoard cb;
    StateInfo st;

    for (int i = 0; i < NPOSITIONS; i++) {

        if (fgets(line, 256, f) == NULL) {
            exit(EXIT_FAILURE);
        }

        if (strstr(line, "1-0")) entries[i].result = 1;
        else if (strstr(line, "0-1")) entries[i].result = 0;
        else if (strstr(line, "1/2-1/2")) entries[i].result = 0.5;
        else {
            std::cout << "Error on parse" << std::endl; 
            exit(EXIT_FAILURE);
        }

        cb.fenToBoard(line, st);
        initSingleEntry(entries[i], cb);
        if (i % 100000 == 0) {
            std::cout << "Initialised " << i << " of " << NPOSITIONS << std::endl;
        }
    }

    fclose(f);
}

void Tuner::initSingleEntry(TuningEntry& entry, ChessBoard& cb) {
    double phase = 4 * countBits(cb.pieces(QUEEN)) +
                2 * countBits(cb.pieces(ROOK)) +
                1 * countBits(cb.pieces(BISHOP)) +
                1 * countBits(cb.pieces(KNIGHT));
    entry.phase[mg] = phase / 24;
    entry.phase[eg] = 1.0 - phase / 24;
    entry.origEval = Evaluation::evaluate(cb);
    entry.tEval = entry.origEval;
    if (cb.colourToMove() == BLACK) {
        entry.tEval = -entry.origEval;
        entry.origEval = -entry.origEval;
    }
    initCoeffs(entry, cb);

}

void Tuner::initCoeffs(TuningEntry& entry, ChessBoard& cb) {
    int idx;
    int coeffs[NPARAMS][COLOURNB];
    memset(&Evaluation::trace, 0, sizeof(EvalTrace));
    Evaluation::traceEval<WHITE>(cb);
    Evaluation::traceEval<BLACK>(cb);
    if (TUNEMATERIAL) {
        for (idx = 0; idx < KING - 1; idx++) {
            coeffs[idx][WHITE] = countBits(cb.pieces(WHITE, PieceType(idx + 1)));
            coeffs[idx][BLACK] = countBits(cb.pieces(BLACK, PieceType(idx + 1)));
        }
    }

    if (TUNEPSQT) {
        for (int j = PAWN; j <= KING; j++) {
            for (int k = 0; k < 64; k++) {
                int rank = k / 8;
                int file = k % 8;
                coeffs[idx][WHITE] = cb.pieceOn((7 - rank)* 8 + file) == makePiece(WHITE, PieceType(j));
                coeffs[idx][BLACK] = cb.pieceOn(k) == makePiece(BLACK, PieceType(j));
                idx++;
            }
        }
    }

    if (TUNEPAWNSTRUCT) {
        for (int j = 0; j < 8; j++) {
            coeffs[idx][WHITE] = Evaluation::trace.connected[WHITE][j];
            coeffs[idx][BLACK] = Evaluation::trace.connected[BLACK][j];
            idx++;
        }
        for (int j = 0; j < 8; j++) {
            coeffs[idx][WHITE] = Evaluation::trace.passed[WHITE][j];
            coeffs[idx][BLACK] = Evaluation::trace.passed[BLACK][j];
            idx++;
        }
        coeffs[idx][WHITE] = Evaluation::trace.isolated[WHITE];
        coeffs[idx][BLACK] = Evaluation::trace.isolated[BLACK];
        idx++;

        coeffs[idx][WHITE] = Evaluation::trace.doubled[WHITE];
        coeffs[idx][BLACK] = Evaluation::trace.doubled[BLACK];
        idx++;

        coeffs[idx][WHITE] = Evaluation::trace.unsupported[WHITE];
        coeffs[idx][BLACK] = Evaluation::trace.unsupported[BLACK];
        idx++;

        coeffs[idx][WHITE] = Evaluation::trace.supported[WHITE];
        coeffs[idx][BLACK] = Evaluation::trace.supported[BLACK];
        idx++;

        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 8; k++) {
                coeffs[idx][WHITE] = Evaluation::trace.shelter[WHITE][j][k];
                coeffs[idx][BLACK] = Evaluation::trace.shelter[BLACK][j][k];
                idx++;
            }
        }

        for (int k = 0; k < 8; k++) {
            coeffs[idx][WHITE] = Evaluation::trace.blockedStorm[WHITE][k];
            coeffs[idx][BLACK] = Evaluation::trace.blockedStorm[BLACK][k];
            idx++;
        }

        for (int k = 0; k < 8; k++) {
            coeffs[idx][WHITE] = Evaluation::trace.unblockedStorm[WHITE][k];
            coeffs[idx][BLACK] = Evaluation::trace.unblockedStorm[BLACK][k];
            idx++;
        }

        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                coeffs[idx][WHITE] = Evaluation::trace.openFile[WHITE][j][k];
                coeffs[idx][BLACK] = Evaluation::trace.openFile[BLACK][j][k];
                idx++;
            }
        }
    }

    if (TUNEKINGSAFETY) {
        for (int j = 0; j < 100; j++) {
            coeffs[idx][WHITE] = Evaluation::trace.safety[WHITE][j];
            coeffs[idx][BLACK] = Evaluation::trace.safety[BLACK][j];
            idx++;
        }
    }

    if (TUNEMOBILITY) {
        coeffs[idx][WHITE] = Evaluation::trace.rookOnOpenFile[WHITE];
        coeffs[idx][BLACK] = Evaluation::trace.rookOnOpenFile[BLACK];
        idx++;
        for (int i = 0; i < 9; i++) {
            coeffs[idx][WHITE] = Evaluation::trace.knightMob[WHITE][i];
            coeffs[idx][BLACK] = Evaluation::trace.knightMob[BLACK][i];
            idx++;
        }
        for (int i = 0; i < 14; i++) {
            coeffs[idx][WHITE] = Evaluation::trace.bishopMob[WHITE][i];
            coeffs[idx][BLACK] = Evaluation::trace.bishopMob[BLACK][i];
            idx++;
        }
        for (int i = 0; i < 15; i++) {
            coeffs[idx][WHITE] = Evaluation::trace.rookMob[WHITE][i];
            coeffs[idx][BLACK] = Evaluation::trace.rookMob[BLACK][i];
            idx++;
        }
        for (int i = 0; i < 28; i++) {
            coeffs[idx][WHITE] = Evaluation::trace.queenMob[WHITE][i];
            coeffs[idx][BLACK] = Evaluation::trace.queenMob[BLACK][i];
            idx++;
        }
    }

    if (idx != NPARAMS) {
        std::cout << "Error on init Coeff" << std::endl;
        exit(EXIT_FAILURE);
    }
    int length = 0;
    for (int i = 0; i < idx; i++) {
        if (coeffs[i][WHITE] != coeffs[i][BLACK]) {
            length++;
        }
    }
    entry.nTerms = length;
    entry.coeffs = (Coeffs*)malloc(length * sizeof(Coeffs));
    int coeffIdx = 0;
    for (int i = 0; i < idx; i++) {
        if (coeffs[i][WHITE] != coeffs[i][BLACK]) {
            entry.coeffs[coeffIdx].white = coeffs[i][WHITE];
            entry.coeffs[coeffIdx].black = coeffs[i][BLACK];
            entry.coeffs[coeffIdx].index = i;
            coeffIdx++;
        }
    }

}

double Tuner::linearEval(TuningEntry& entry, double params[NPARAMS][PHASENB]) {
    double score[PHASENB] = {0.0};
    for (int i = 0; i < entry.nTerms; i++) {
        // Add any changes to the score
        score[mg] += params[entry.coeffs[i].index][mg] * (entry.coeffs[i].white - entry.coeffs[i].black);
        score[eg] += params[entry.coeffs[i].index][eg] * (entry.coeffs[i].white - entry.coeffs[i].black);
    }

    // The score represents the change in score from when the entry was initialised
    return score[mg] * entry.phase[mg] + score[eg] * entry.phase[eg] + entry.origEval;
}

double Tuner::minimizeK(TuningEntry* entries) {
    return 0.006889;
    double start = 0.0, end = 10, step = 1.0;
    double curr = start, error;
    double best = staticEvalError(entries, start);
    std::cout << "Minimizing K with precision: " << KPRECISION << std::endl;
    for (int i = 0; i < KPRECISION; i++) {
        std::cout << "Epoch: " << i << " Error: " << best << std::endl;
        curr = start - step;
        while (curr < end) {
            curr += step;
            error = staticEvalError(entries, curr);
            if (error <= best) {
                best = error;
                start = curr;
            }
        }
        end = start + step;
        start = start - step;
        step = step / 10.0;
    }

    return start;
}

double Tuner::staticEvalError(TuningEntry* entries, double k) {
    double total = 0;
    for (int i = 0; i < NPOSITIONS; i++) {
        total += pow(entries[i].result - sigmoid(k, entries[i].origEval), 2);
    }
    return total / (double) NPOSITIONS;
}

double Tuner::tunedEvalError(TuningEntry* entries, double params[NPARAMS][PHASENB], double k) {
    double sum = 0;
    // The equation we use for the error is 1/n * sum((R - sigmoid(E))^2)
    // N is the number of positions we are tuning against, R is the final result of the given
    // Tuning position (0 = black win, 0.5 = draw, 1 = white win) E is the evaluation
    // given the current tuning epoch.
    for (int i = 0; i < NPOSITIONS; i++) {
        entries[i].tEval = linearEval(entries[i], params);
        sum += pow(entries[i].result - sigmoid(k, entries[i].tEval), 2);
    }
    return sum / (double)NPOSITIONS;
}

double Tuner::sigmoid(double k, int eval) {
    return 1.0 / (1.0 + exp(-k * eval));
}

void Tuner::computeGradients(TuningEntry* entries, double params[NPARAMS][PHASENB], double gradient[NPARAMS][PHASENB], double k) {
    for (int i = 0; i < NPOSITIONS; i++) {
        // We actually are computing the sum of the gradients. We take the average later to avoid precision loss when dividing
        computeSingleGradient(entries[i], params, gradient, k);
    }
}

void Tuner::computeSingleGradient(TuningEntry& entry, double params[NPARAMS][PHASENB], double gradient[NPARAMS][PHASENB], double k) {
    // The gradient is a vector of the partials of the sum MSE with respect to each parameter
    double E = linearEval(entry, params);
    double S = sigmoid(k, E);
    double X = (entry.result - S) * S * (1 - S);
    double mgBase = X * entry.phase[mg];
    double egBase = X * entry.phase[eg];

    for (int i = 0; i < entry.nTerms; i++) {
        gradient[entry.coeffs[i].index][mg] += mgBase * (entry.coeffs[i].white - entry.coeffs[i].black);
        gradient[entry.coeffs[i].index][eg] += egBase * (entry.coeffs[i].white - entry.coeffs[i].black);
    }

}