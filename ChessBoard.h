#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <string>
#include <iostream>
#include <bitset>
#include <unordered_map>
#include "Types.h"
#include "BitBoards.h"



// Inspired by stockFish's stateInfo struct.
// Used to restore the position of a board on undoMove
struct StateInfo {
    char castlingRights;
    char epSquare;

    Piece captured;
    StateInfo* previous;
};
 
class ChessBoard {

    public:
    // fenString constructor
    ChessBoard(std::string fenString, StateInfo& si);
    // return piece code at specific index
    Piece pieceOn(int sq);
    Colour colourToMove();
    // Performs a move on the board. Does not check if it is a legal chess move.
    void makeMove(Move move, StateInfo& si);
    // prints a bitboard
    static void printBoard(bitBoard bb);
    char epSquare() const;

    // Returns the bitboard of all attackers of one colour of a certain square
    template<Colour c>
    bitBoard getAttackers(int sq);

    bool canCastle(CastlingRights cr) const;

    bitBoard pieces(Colour c, PieceType pt);
    bitBoard pieces(Colour c);
    bitBoard pieces();

    private:

    // Fields 
    Colour colToMove;
    StateInfo* st;
    Piece board[64];   
    bitBoard piecesByType[PIECENB];
    bitBoard piecesByColour[2];
    // The Occupancy of all pieces
    bitBoard allPieces;

    void fenToBoard(std::string fenString);
    void initBoard(StateInfo& si);
    // Assumes to is an empty square
    void movePiece(int from, int to);
    void removePiece(int sq);
    void putPiece(Piece pc, int sq);
    
};

inline Colour ChessBoard::colourToMove() {
    return colToMove;
}

inline bitBoard ChessBoard::pieces(Colour c, PieceType pt) {
    return piecesByType[(c << 3) + pt];
}

inline bitBoard ChessBoard::pieces(Colour c) {
    return piecesByColour[c];
}

inline bitBoard ChessBoard::pieces() {
    return allPieces;
}

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