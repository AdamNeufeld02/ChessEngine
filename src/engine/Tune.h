#ifndef TUNE_H
#define TUNE_H

#include <math.h>
#include <string.h>
#include "Evaluation.h"

#define NPOSITIONS 100
#define TUNINGFILE ""
#define KPRECISION 10
#define TUNEMATERIAL 1
#define TUNEPSQT 1
#define TUNEPAWNSTRUCT 1
#define TUNEKINGSAFETY 1
#define TUNEMOBILITY 1

#define NMATERIALPAR 5
#define NPSQTPAR 384
#define NPAWNSTRUCTPAR 71
#define NKINGSAFETYPAR 100
#define NTUNEMOBILITYPAR 66

// We say one parameter is both its mid game and end game counterpart
#define NPARAMS (TUNEMATERIAL * NMATERIALPAR) + (TUNEPSQT * NPSQTPAR) + (TUNEPAWNSTRUCT * NPAWNSTRUCTPAR) + (TUNEKINGSAFETY * NKINGSAFETYPAR) + (TUNEMOBILITY * NTUNEMOBILITYPAR)

struct Coeffs
{
    uint8_t index;
    int white, black;
};

struct TuningEntry
{
    Coeffs* coeffs;
    int sEval, result, nTerms;
    double phase[PHASENB];
    Score tEval;
};

namespace Tuner {
    void runTuner();
    void initEntries(TuningEntry* entries);
    void initSingleEntry(TuningEntry& entry, ChessBoard& cb);
    void initParams(int** params);
    void initCoeffs(TuningEntry& entry, ChessBoard& cb);

    double sigmoid(double k, int eval);
    double minimizeK(TuningEntry* entries);
    double staticEvalError(TuningEntry* entries, double k);
    double tunedEvalError(TuningEntry* entries, double k);
    double linearEval(TuningEntry* entry, double** params);
    
    void computeGradients(TuningEntry* entries);
    void computeSingleGradient(TuningEntry* entry, double k);
    void updateConsole(TuningEntry* entries);
}



#endif