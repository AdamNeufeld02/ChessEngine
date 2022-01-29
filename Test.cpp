#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "MoveGenerator.h"
#include "ChessBoard.h"
#include "BitBoards.h"
#include "Search.h"
#include "MovePick.h"
#include "Evaluation.h"
#include <bitset>
#include <chrono>

TEST_CASE("ChessBoard::FenString constructor", "[Weight=1][part=ChessBoard]") {
    BitBoards::precomputeAttackSets();
    Evaluation::init();
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
    ScoredMove moves[MAXMOVES];
    ScoredMove* end = MoveGenerator::generateMoves(cb, moves, false);
    MovePick mp = MovePick(moves, end - moves, cb);
    std::cout << moves[0].score << std::endl;
    Move move = mp.getNext();
    int i = 0;
    while(move != NOMOVE) {
        std::cout << moves[i].score << std::endl;
        move = mp.getNext();
    }
}