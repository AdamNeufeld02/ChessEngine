#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <string>
#include <iostream>
#include <bitset>
#include <unordered_map>
#include "Types.h"
#include "BitBoards.h"
#include "Evaluation.h"
#include "Zobrist.h"


// Inspired by stockFish's stateInfo struct.
// Used to restore the position of a board on undoMove
struct StateInfo {
    char castlingRights;
    char epSquare;
    zobristKey key;

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
    Score getPSQT(Colour col);

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

    zobristKey key();

    // Calculates what the hash key will be after the move is made.
    // Used in prefetch
    zobristKey keyAfter(Move move);

    bool onOpenFile(Colour col, int sq);

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
    Score psqtv[2];

    void fenToBoard(std::string fenString);
    void initBoard(StateInfo& si);

    void updateChecksAndPins(Colour toMove);
    bitBoard blockersForSq(int sq, Colour col, bitBoard& pinners);
    // Assumes to is an empty square
    void movePiece(int from, int to);
    void removePiece(int sq);
    void putPiece(Piece pc, int sq);
    
};

inline bool ChessBoard::onOpenFile(Colour col, int sq) {
    return !(pieces(col, PAWN) & fileBB[sq % 8]);
}

inline Score ChessBoard::getPSQT(Colour col) {
    return psqtv[col];
}

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

inline zobristKey ChessBoard::key() {
    return st->key;
}

inline void ChessBoard::putPiece(Piece pc, int sq) {
    bitBoard place = squares[sq];
    Colour col = colourOf(pc);
    material[col] += mgVals[typeOf(pc)];
    psqtv[col] += pieceSquareTables[pc][sq];
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
    psqtv[col] -= pieceSquareTables[pc][sq];
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
    Colour col = colourOf(pc);
    psqtv[col] += pieceSquareTables[pc][to] - pieceSquareTables[pc][from];
    allPieces ^= fromTo;
    piecesByType[pc] ^= fromTo;
    piecesByColour[col] ^= fromTo;
    board[from] = EMPTY;
    board[to] = pc;
}
#endif