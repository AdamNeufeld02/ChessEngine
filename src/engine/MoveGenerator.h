#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include <iostream>
#include "ChessBoard.h"
#include "Types.h"
#include "BitBoards.h"
// A chess move generator utilizing magic bitboards
// For more info on magic bitboards visit: https://www.chessprogramming.org/Magic_Bitboards

namespace MoveGenerator {
    // Generates pseudo legal chess moves for the chessBoard given based on which side is to move
    // Places all moves in the move list provided
    // Returns a pointer to the end of the list of generated moves
    ScoredMove* generateMoves(ChessBoard& chessBoard, ScoredMove* moves, bool onlyCaptures);

    template<GenType t>
    ScoredMove* generateAllMoves(ChessBoard& chessBoard, ScoredMove* moves);
    template<GenType t, Colour us>
    ScoredMove* generateMoves(ChessBoard& chessBoard, ScoredMove* moves);
    template<PieceType pt, Colour us>
    ScoredMove* generateMoves(ChessBoard& chessBoard, ScoredMove* moves, bitBoard targets);
    // generates moves for pawns including enpassent, promotions and double pushes
    template<GenType t, Colour us>
    ScoredMove* generatePawnMoves(ChessBoard& chessBoard, ScoredMove* moves, bitBoard targets);
    //generates all king moves
    template<GenType t, Colour us>
    ScoredMove* generateKingMoves(ChessBoard& ChessBoard, ScoredMove* moves);
}

#endif