#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "MoveGenerator.h"
#include "ChessBoard.h"
#include <bitset>
#include <chrono>

TEST_CASE("ChessBoard::FenString constructor", "[Weight=1][part=ChessBoard]") {
    std::string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    StateInfo state;
    ChessBoard cb = ChessBoard(startingFen, state);
    cb.printBoard(cb.allPieces);
    REQUIRE(cb.whiteToMove);
    REQUIRE(cb.countBits(cb.allPieces) == 32); 
}

TEST_CASE("MoveGenerator::InitOccMasks", "[Weight=1][part=MoveGenerator]") {
    std::string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    StateInfo state;
    ChessBoard cb = ChessBoard(startingFen, state);
    MoveGenerator* mg = new MoveGenerator();
    Move moves[MAXMOVES]; 
    Move* endmoves;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100000000; i++) {
        endmoves = mg->generateMoves(&cb, moves);
    }
    auto stop = std::chrono::high_resolution_clock::now();

    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << ms_int.count() << std::endl;
    int length = endmoves - moves;
    REQUIRE(length == 20);
    delete mg;
}