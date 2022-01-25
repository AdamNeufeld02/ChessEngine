#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <string>
#include <iostream>
#include <bitset>
#include <unordered_map>
#include "Types.h"
#include "BitBoards.h"
#include "Evaluation.h"


// Inspired by stockFish's stateInfo struct.
// Used to restore the position of a board on undoMove
struct StateInfo {
    char castlingRights;
    char epSquare;

    Piece captured;
    bitBoard checkersBB;
    bitBoard pinnersBB;
    bitBoard pinnedBB;
    StateInfo* previous;
};
 
class ChessBoard {

    public:
    // fenString constructor
    ChessBoard(std::string fenString, StateInfo& si);
    // return piece code at specific index
    Piece pieceOn(int sq);
    Colour colourToMove() const;
    // Performs a move on the board. Does not check if it is a legal chess move.
    void doMove(Move move, StateInfo& si);
    // Undos the given move. Must have been the last move played
    void undoMove(Move move);
    // prints a bitboard
    static void printBoard(bitBoard bb);
    char epSquare() const;

    int getMaterial(Colour col);

    // Returns the bitboard of all attackers of one colour of a certain square
    bitBoard getAttackers(int sq, Colour c);

    // returns the bitboard of sliding attacks that the given colour attacks for the given occ
    bitBoard getSlidingAttacks(bitBoard occ);

    bool canCastle(CastlingRights cr) const;

    bitBoard pieces(Colour c, PieceType pt);
    bitBoard pieces(Colour c);
    bitBoard pieces();

    bitBoard checkers();
    bitBoard pinners();
    bitBoard pinned();

    private:

    // Fields 
    Colour colToMove;
    StateInfo* st;
    Piece board[64];   
    bitBoard piecesByType[PIECENB];
    bitBoard piecesByColour[2];
    // The Occupancy of all pieces
    bitBoard allPieces;
    int material[2];

    void fenToBoard(std::string fenString);
    void initBoard(StateInfo& si);

    void updateChecksAndPins(Colour toMove);
    bitBoard blockersForSq(int sq, Colour col, bitBoard& pinners);
    // Assumes to is an empty square
    void movePiece(int from, int to);
    void removePiece(int sq);
    void putPiece(Piece pc, int sq);
    
};

inline int ChessBoard::getMaterial(Colour col) {
    return material[col];
}

inline Colour ChessBoard::colourToMove() const {
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

inline bitBoard ChessBoard::checkers() {
    return st->checkersBB;
}

inline bitBoard ChessBoard::pinners() {
    return st->pinnersBB;
}

inline bitBoard ChessBoard::pinned() {
    return st->pinnedBB;
}

inline bool ChessBoard::canCastle(CastlingRights cr) const {
    return st->castlingRights & cr;
}

inline void ChessBoard::putPiece(Piece pc, int sq) {
    bitBoard place = squares[sq];
    Colour col = colourOf(pc);
    material[col] += mgVals[typeOf(pc)];
    board[sq] = pc;
    piecesByType[pc] |= place;
    piecesByColour[col] |= place;
    allPieces |= place;
}

inline void ChessBoard::removePiece(int sq) {
    bitBoard place = squares[sq];
    Piece pc = board[sq];
    Colour col = colourOf(pc);
    material[col] -= mgVals[typeOf(pc)];
    board[sq] = EMPTY;
    piecesByType[pc] ^= place;
    piecesByColour[col] ^= place;
    allPieces ^= place;
}

inline Piece ChessBoard::pieceOn(int sq) {
    return board[sq];
}

inline void ChessBoard::movePiece(int from, int to) {
    bitBoard fromTo = (squares[from] | squares[to]);
    Piece pc = board[from];
    allPieces ^= fromTo;
    piecesByType[pc] ^= fromTo;
    piecesByColour[colourOf(pc)] ^= fromTo;
    board[from] = EMPTY;
    board[to] = pc;
}
#endif