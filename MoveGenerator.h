#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include <iostream>
#include "ChessBoard.h"
#include "Types.h"
#include "BitBoards.h"
// A chess move generator utilizing magic bitboards
// For more info on magic bitboards visit: https://www.chessprogramming.org/Magic_Bitboards

// TODO:
//  - filter illegal castles
//  - Clean up

namespace MoveGenerator {
    // Generates pseudo legal chess moves for the chessBoard given based on which side is to move
    // Places all moves in the move list provided
    // Returns a pointer to the end of the list of generated moves
    Move* generateMoves(ChessBoard& chessBoard, Move* moves);
    // Function that was used to generate magics for rooks and bishops.
    // Heavily inspired by the routine found on https://www.chessprogramming.org/Looking_for_Magics
    bitBoard generateMagicNumber(int square, int rook);

    template<GenType t>
    Move* generateAllMoves(ChessBoard& chessBoard, Move* moves);
    template<GenType t, Colour us>
    Move* generateMoves(ChessBoard& chessBoard, Move* moves);
    template<PieceType pt, Colour us>
    Move* generateMoves(ChessBoard& chessBoard, Move* moves, bitBoard targets);
    // generates moves for pawns including enpassent, promotions and double pushes
    template<GenType t, Colour us>
    Move* generatePawnMoves(ChessBoard& chessBoard, Move* moves, bitBoard targets);
    //generates all king moves
    template<GenType t, Colour us>
    Move* generateKingMoves(ChessBoard& ChessBoard, Move* moves);
    
    unsigned int XORShift32Rand();
    // generates 64 bit number with low number of set bits (ideal candidate)
    bitBoard getMagicNumberCandidate();
    bitBoard genRand64();
    
}

#endif