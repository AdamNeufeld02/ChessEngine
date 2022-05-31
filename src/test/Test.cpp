#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "MoveGenerator.h"
#include "ChessBoard.h"
#include "BitBoards.h"
#include "Search.h"
#include "MovePick.h"
#include "Evaluation.h"
#include "TransposTable.h"
#include "Tune.h"
#include "Threads.h"
#include <bitset>
#include <chrono>

void testKey(ChessBoard& cb) {
    zobristKey key = 0;
    for (int i = 0; i< 64; i++) {
        if (cb.pieceOn(i)) {
            key ^= Zobrist::psq[cb.pieceOn(i)][i];
        }
    }
    if (cb.colourToMove() == BLACK) key ^= Zobrist::colToMove;
    if (cb.epSquare() != -1) key ^= Zobrist::epSquare[(unsigned)cb.epSquare()];
    key ^= Zobrist::castlingKeys[cb.getCR()];
    REQUIRE(key == cb.key());
}

TEST_CASE("ChessBoard::FenString constructor", "[Weight=1][part=ChessBoard]") {
    BitBoards::precomputeAttackSets();
    Evaluation::init();
    Zobrist::init();
    std::string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    StateInfo state;
    ChessBoard cb = ChessBoard(startingFen, state);
    cb.printBoard(cb.pieces());
    REQUIRE(cb.colourToMove() == WHITE);
    REQUIRE(countBits(cb.pieces()) == 32); 
}

TEST_CASE("MoveGenerator::InitOccMasks", "[Weight=1][part=MoveGenerator]") {
    std::string startingFen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
    StateInfo state;
    ChessBoard cb = ChessBoard(startingFen, state);
    zobristKey key = 0;
    for (int i = 0; i < 64; i++) {
        if(typeOf(cb.pieceOn(i)) == PAWN) {
            key ^= Zobrist::psq[cb.pieceOn(i)][i];
        } else if (typeOf(cb.pieceOn(i)) == KING) {
            key ^= Zobrist::psq[cb.pieceOn(i)][i];
        }
    }
    REQUIRE(key == cb.pawnKey());

    // Simple SEE tests
    REQUIRE(cb.seeGE(makeMove(12, 40), 330));
    REQUIRE(!(cb.seeGE(makeMove(21, 45), -599)));
    REQUIRE(cb.seeGE(makeMove(35, 44), 0));
    REQUIRE(!(cb.seeGE(makeMove(36, 53), -199)));
}

TEST_CASE("TransPosTable::Entry", "[Weight=1][part=TransPosTable]") {
    zobristKey key = 0;
    Move move = NOMOVE;
    Type type = Lower;
    int depth = 18;
    int score = -Infinity;
    int eval = -1;
    Entry entry;
    bool ttPV = true;
    entry.save(key, score, eval, depth, move, type, ttPV);
    uint64_t data = entry.data;

    REQUIRE(key ^ data == entry.keyXData);
    REQUIRE(move == unpackMove(data));
    REQUIRE(eval == unpackEval(data));
    REQUIRE(depth == unpackDepth(data));
    REQUIRE(score == unpackScore(data));
    REQUIRE(type == unpackType(data));
    REQUIRE(ttPV == unpackTTPV(data));
}

TEST_CASE("ChessBoard::Key", "[Weight=1][part=ChessBoard]") {
    std::string startingFen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
    StateInfo state;
    ChessBoard cb = ChessBoard(startingFen, state);
    ScoredMove moves[MAXMOVES];
    ScoredMove* end;
    StateInfo si;
    testKey(cb);
    std::cout << "Initial Key success" << std::endl;

    for (int i = 0; i < 64; i++) {
        end = MoveGenerator::generateMoves(cb, moves, false);
        int length = (end - moves);
        if (length > 0) {
            cb.doMove(moves[0].move, si);
            testKey(cb);
        }
    }
}

TEST_CASE("Tune::initCoeffs", "[Weight=1][part=Tune]") {
    std::ifstream inFile(TUNINGFILE); 
    int cnt = std::count(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), '\n');
    std::cout << "Num Positions: " << cnt << std::endl;
    Tuner::runTuner();
    StateInfo si;
    std::string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string altPos[6] ={"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ",
                            "8/3kp3/1p3bp1/8/1B1P1PK1/4P3/8/8 w - - 0 40 [0.0]",
                            "8/8/p7/b1B5/2kPR3/1p2P1r1/4K3/8 w - - 0 53 [0.0]",
                            "r4rk1/1pq2pbp/p2pbnp1/2p1n3/3P4/2P1B1P1/PPN1PNBP/R2Q1RK1 w - - 0 16 [0.0]",
                            "6r1/8/p2bp2R/Pp1k1p2/3P4/4PKP1/8/4B3 w - - 16 47 [0.0]",
                            "8/1r6/4K3/p1B3b1/2kP4/8/1p6/1R6 w - - 0 56 [0.0]"};   

    ChessBoard cb = ChessBoard(startingFen, si);

    std::cout << cb.boardToFen() << std::endl;
    cb.fenToBoard(cb.boardToFen(), si);
    TuningEntry entry;
    Tuner::initSingleEntry(entry, cb);
    REQUIRE(entry.nTerms == 0);

    double params[NPARAMS][PHASENB] = {0};
    Tuner::setParams(params);
    for (int i = 0; i < 6; i++) {
        memset(&Evaluation::trace, 0, sizeof(EvalTrace));
        cb.fenToBoard(altPos[i], si);
        cb.fenToBoard(cb.boardToFen(), si);
        std::cout << cb.boardToFen() << std::endl;
        Tuner::initSingleEntry(entry, cb);
        int sEval = entry.origEval;
        entry.origEval = 0;
        int lEval = Tuner::linearEval(entry, params);
        REQUIRE(lEval == sEval);
    }
    double old[NPARAMS][PHASENB] = {0};
    Tuner::writeParams(params, old);
}