#ifndef TYPES_H
#define TYPES_H

enum Colour {
    WHITE, BLACK
};

enum PieceType {
    NONE,
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};

enum Piece {
    EMPTY, 
    WHITEPAWN = PAWN, WHITEKNIGHT, WHITEBISHOP, WHITEROOK, WHITEQUEEN, WHITEKING,
    BLACKPAWN = WHITEPAWN + 8, BLACKKNIGHT, BLACKBISHOP, BLACKROOK, BLACKQUEEN, BLACKKING,
    PIECENB
};

enum FLAGS {
    NOFLAG, ENPASSENT, DOUBLEPUSH, 
    KINGCASTLE =  0b00000100, 
    QUEENCASTLE=  0b00001000
};

// Taken from https://github.com/official-stockfish/Stockfish/blob/master/src/position.h
enum CastlingRights {
    NO_CASTLING,
    WHITE_OO,
    WHITE_OOO = WHITE_OO << 1,
    BLACK_OO  = WHITE_OO << 2,
    BLACK_OOO = WHITE_OO << 3,

    KING_SIDE      = WHITE_OO  | BLACK_OO,
    QUEEN_SIDE     = WHITE_OOO | BLACK_OOO,
    WHITE_CASTLING = WHITE_OO  | WHITE_OOO,
    BLACK_CASTLING = BLACK_OO  | BLACK_OOO,
    ANY_CASTLING   = WHITE_CASTLING | BLACK_CASTLING,

    CASTLING_RIGHT_NB = 16
};

// A struct representing an encoded move
// captToFrom:
// bits (0-5) From
// bits (6-11) to
// bits (12-15) Captured piece code
// promflags
// bits (0-3) flags
// bits(4- 7) Promoted Piece Code
struct Move {
    short captToFrom;
    char promFlags;
};

template<FLAGS f>
Move makeMove(int from, int to, Piece capt, Piece prom) {
    Move move;
    move.captToFrom = ((capt << 12) + (to << 6) + from);
    move.promFlags = ((prom << 4) + f);
    return move;
}

constexpr int getFrom(Move move) {
    return move.captToFrom & 0x3f;
}

constexpr int getTo(Move move) {
    return (move.captToFrom >> 6) & 0x3f; 
}

constexpr FLAGS getFlag(Move move) {
    return FLAGS(move.promFlags & 0xf);
}

constexpr Piece getCapt(Move move) {
    return Piece((move.captToFrom >> 12) & 0xf);
}

constexpr Piece getProm(Move move) {
    return Piece((move.promFlags >> 4) & 0xf);
}

constexpr int MAXMOVES = 256;

inline Colour colourOf(Piece pc) {
    return Colour(pc >> 3);
}

inline PieceType typeOf(Piece pc) {
    return PieceType(pc & 7);
}

#endif