#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <cstdint>
#include <string>
#include <iostream>
#include <bitset>
#include <unordered_map>
#include "Types.h"

typedef uint64_t bitBoard;

// Inspired by stockFish's stateInfo struct.
// Used to restore the position of a board on undoMove
struct StateInfo {
    char castlingRights;
    char epSquare;
    StateInfo* previous;
};
 
class ChessBoard {

    public:
    // fenString constructor
    ChessBoard(std::string fenString, StateInfo& si);
    // return piece code at specific index
    Piece pieceOn(int sq);
    // Performs a move on the board. Does not check if it is a legal chess move.
    // Updates the enpassent, castle, and whiteToMove fields based on the move played
    void makeMove(Move move, StateInfo& si);
    // sets the bit on the bitboard to 0
    static void popBit(bitBoard& bb, int index);
    // sets bit on bitboard to one
    static void setBit(bitBoard& bb, int index);
    // prints a bitboard
    static void printBoard(bitBoard bb);
    // counts set bits on board
    static int countBits(bitBoard bb);

    char epSquare() const;

    bool canCastle(CastlingRights cr) const;
    
    // Fields 
    bool whiteToMove;
    StateInfo* st;
    Piece board[64];   
    bitBoard piecesByType[PIECENB];
    bitBoard piecesByColour[2];
    // The Occupancy of all pieces
    bitBoard allPieces;

    private:
    // Rank and File masks Currently not used
    const bitBoard rankMasks[8] = {
        0xff, 0xff00, 0xff0000, 0xff000000,
	    0xff00000000, 0xff0000000000, 0xff000000000000, 0xff00000000000000
    };
    const bitBoard fileMasks[8] = {
        0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808,
        0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080
    };


    void fenToBoard(std::string fenString);
    void initBoard(StateInfo& si);
    // Assumes to is an empty square
    void movePiece(int from, int to);
    void removePiece(int sq);
    void putPiece(Piece pc, int sq);
    
};

inline char ChessBoard::epSquare() const {
    return st->epSquare;
}

inline bool ChessBoard::canCastle(CastlingRights cr) const {
    return st->castlingRights & cr;
}

inline void ChessBoard::putPiece(Piece pc, int sq) {
    bitBoard place = (bitBoard)1 << sq;
    board[sq] = pc;
    piecesByType[pc] |= place;
    piecesByColour[colourOf(pc)] |= place;
    allPieces |= place;
}

inline void ChessBoard::removePiece(int sq) {
    bitBoard place = (bitBoard)1 << sq;
    Piece pc = board[sq];
    board[sq] = EMPTY;
    piecesByType[pc] ^= place;
    piecesByColour[colourOf(pc)] ^= place;
    allPieces ^= place;
}

inline Piece ChessBoard::pieceOn(int sq) {
    return board[sq];
}

inline void ChessBoard::movePiece(int from, int to) {
    bitBoard fromTo = ((bitBoard)1 << from) | ((bitBoard) 1 << to);
    Piece pc = board[from];
    allPieces ^= fromTo;
    piecesByType[pc] ^= fromTo;
    piecesByColour[colourOf(pc)] ^= fromTo;
    board[from] = EMPTY;
    board[to] = pc;
}
#endif