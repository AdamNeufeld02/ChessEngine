#include "Tune.h"

void Tuner::runTuner() {
    
}

void Tuner::initEntries(TuningEntry* entries) {
    char line[256];
    FILE* f = fopen(TUNINGFILE, "r");
    ChessBoard cb;
    StateInfo st;
    Evaluation::doTrace = true;

    for (int i = 0; i < NPOSITIONS; i++) {

        if (fgets(line, 256, f) == NULL) {
            exit(EXIT_FAILURE);
        }

        if (strstr(line, "[1.0]")) entries[i].result = 1;
        else if (strstr(line, "[0.0]")) entries[i].result = 0;
        else if (strstr(line, "[0.5]")) entries[i].result = 0.5;
        else {
            std::cout << "Error on parse" << std::endl; 
            exit(EXIT_FAILURE);
        }

        cb.fenToBoard(line, st);
        initSingleEntry(entries[i], cb);
    }

    fclose(f);
}

void Tuner::initSingleEntry(TuningEntry& entry, ChessBoard& cb) {
    int phase = 4 * countBits(cb.pieces(QUEEN)) +
                2 * countBits(cb.pieces(ROOK)) +
                1 * countBits(cb.pieces(BISHOP)) +
                1 * countBits(cb.pieces(KNIGHT));
    entry.phase[mg] = phase / 24;
    entry.phase[eg] = 1.0 - phase / 24;
    memset(&Evaluation::trace, 0, sizeof(EvalTrace));
    entry.sEval = Evaluation::evaluate(cb);
    initCoeffs(entry, cb);

}

void Tuner::initCoeffs(TuningEntry& entry, ChessBoard& cb) {
    int idx;
    int coeffs[NPARAMS][COLOURNB];
    if (TUNEMATERIAL) {
        for (idx = 0; idx < KING - 1; idx++) {
            coeffs[idx][WHITE] = countBits(cb.pieces(WHITE, PieceType(idx + 1)));
            coeffs[idx][BLACK] = countBits(cb.pieces(BLACK, PieceType(idx + 1)));
        }
    }

    if (TUNEPSQT) {
        for (int j = PAWN; j <= KING; j++) {
            for (int k = 0; k < 64; k++) {
                coeffs[idx][WHITE] = cb.pieceOn(k) == makePiece(WHITE, PieceType(j));
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
            entry.coeffs[coeffIdx].index = coeffIdx;
            coeffIdx++;
        }
    }

}

double Tuner::linearEval(TuningEntry* entry, double** params) {
    double score[PHASENB] = {0.0};
    for (int i = 0; i < entry->nTerms; i++) {
        score[mg] += params[mg][entry->coeffs->index] * (entry->coeffs->white - entry->coeffs->black);
        score[eg] += params[eg][entry->coeffs->index] * (entry->coeffs->white - entry->coeffs->black);
    }

    return score[mg] * entry->phase[mg] + score[eg] * entry->phase[eg];
}

double Tuner::minimizeK(TuningEntry* entries) {
    double start = 0.0, end = 10, step = 1.0;
    double curr = start, error;
    double best = staticEvalError(entries, start);

    for (int i = 0; i < KPRECISION; i++) {
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
        total += pow(entries[i].result - sigmoid(k, entries[i].sEval), 2);
    }
    return total / (double) NPOSITIONS;
}

double Tuner::sigmoid(double k, int eval) {
    return 1.0 / (1.0 + exp(-k * eval));
}

void Tuner::computeSingleGradient(TuningEntry* entry, double k) {

}