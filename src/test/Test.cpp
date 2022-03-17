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
    if (cb.epSquare() != -1) key ^= Zobrist::epSquare[cb.epSquare()];
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
    StateInfo si;
    std::string startingFen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
    ChessBoard cb = ChessBoard(startingFen, si);
    Evaluation::doTrace = true;
    TuningEntry entry;
    entry.sEval = Evaluation::evaluate(cb);
    Tuner::initCoeffs(entry, cb);

    std::cout << std::log(10) << std::endl;
}