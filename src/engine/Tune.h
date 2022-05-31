#ifndef TUNE_H
#define TUNE_H

#include <math.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "Evaluation.h"

#define MAXEPOCHS 40000
#define LEARNINGRATE 1
#define EPISLON 0.00000001
// Training1: 9999740
// Training2: 9996883
// Training3: 9998762
// quiet-labeled.epd: 725000
#define NPOSITIONS 725000
#define TUNINGFILE "quiet-labeled.epd"
#define RESULTFILE "output2.txt"
#define KPRECISION 10
#define UPDATEON 15

#define TUNEMATERIAL 0
#define TUNEPSQT 0
#define TUNEPAWNSTRUCT 1
#define TUNEKINGSAFETY 0
#define TUNEMOBILITY 0

#define NMATERIALPAR 5
#define NPSQTPAR 384
#define NPAWNSTRUCTPAR 72
#define NKINGSAFETYPAR 100
#define NTUNEMOBILITYPAR 67

// We say one parameter is both its mid game and end game counterpart
#define NPARAMS (TUNEMATERIAL * NMATERIALPAR) + (TUNEPSQT * NPSQTPAR) + (TUNEPAWNSTRUCT * NPAWNSTRUCTPAR) + (TUNEKINGSAFETY * NKINGSAFETYPAR) + (TUNEMOBILITY * NTUNEMOBILITYPAR)

struct Coeffs
{
    uint16_t index;
    uint8_t white, black;
};

struct TuningEntry
{
    Coeffs* coeffs;
    int result, nTerms;
    double phase[PHASENB];
    double tEval, origEval;
};

namespace Tuner {
    void runTuner();
    void initEntries(TuningEntry* entries);
    void initSingleEntry(TuningEntry& entry, ChessBoard& cb);
    void initCoeffs(TuningEntry& entry, ChessBoard& cb);
    void setParams(double params[NPARAMS][PHASENB]);

    double sigmoid(double k, int eval);
    double minimizeK(TuningEntry* entries);
    double staticEvalError(TuningEntry* entries, double k);
    double tunedEvalError(TuningEntry* entries, double params[NPARAMS][PHASENB], double k);
    double linearEval(TuningEntry& entry, double params[NPARAMS][PHASENB]);
    
    void computeGradients(TuningEntry* entries, double params[NPARAMS][PHASENB], double gradient[NPARAMS][PHASENB], double k);
    void computeSingleGradient(TuningEntry& entry, double params[NPARAMS][PHASENB], double gradients[NPARAMS][PHASENB], double k);
    void writeParams(double params[NPARAMS][PHASENB], double oldParams[NPARAMS][PHASENB]);
    void dumpParams(double params[NPARAMS][PHASENB], double oldParams[NPARAMS][PHASENB], std::ofstream& f);
}



#endif