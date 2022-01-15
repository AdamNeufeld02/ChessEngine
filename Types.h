#ifndef TYPES_H
#define TYPES_H

typedef uint64_t bitBoard;

// from https://github.com/official-stockfish/Stockfish/blob/master/src/bitboard.h
constexpr bitBoard FileABB = 0x0101010101010101ULL;
constexpr bitBoard FileBBB = FileABB << 1;
constexpr bitBoard FileCBB = FileABB << 2;
constexpr bitBoard FileDBB = FileABB << 3;
constexpr bitBoard FileEBB = FileABB << 4;
constexpr bitBoard FileFBB = FileABB << 5;
constexpr bitBoard FileGBB = FileABB << 6;
constexpr bitBoard FileHBB = FileABB << 7;

constexpr bitBoard Rank1BB = 0xFF;
constexpr bitBoard Rank2BB = Rank1BB << (8 * 1);
constexpr bitBoard Rank3BB = Rank1BB << (8 * 2);
constexpr bitBoard Rank4BB = Rank1BB << (8 * 3);
constexpr bitBoard Rank5BB = Rank1BB << (8 * 4);
constexpr bitBoard Rank6BB = Rank1BB << (8 * 5);
constexpr bitBoard Rank7BB = Rank1BB << (8 * 6);
constexpr bitBoard Rank8BB = Rank1BB << (8 * 7);

enum Colour {
    WHITE, BLACK
};

constexpr Colour operator~(Colour c) {
    return Colour(c ^ 1);
}

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
    NOFLAG, ENPASSENT, PROMOTION, CASTLE
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

enum GenType {
    Legal, PseudoLegal, Evasions, Captures
};

enum Direction {
    NORTH = 8,
    EAST = 1,
    SOUTH = -NORTH,
    WEST = -EAST,
    NORTHEAST = NORTH + EAST,
    SOUTHEAST = SOUTH + EAST,
    NORTHWEST = NORTH + WEST,
    SOUTHWEST = SOUTH + WEST
};

// representing an encoded move
// bits (0-5) From
// bits (6-11) to
// bits (12- 13) promoted Piece code - 2 (KNIGHT = 0, BISHOP = 1, etc)
// bits(14- 15) flags
enum Move : short {
    NOMOVE
};

constexpr Move makeMove(int from, int to, PieceType prom) {
    return Move((PROMOTION << 14) + ((prom - 2) << 12) + (to << 6) + from);
}

template<FLAGS f>
constexpr Move makeMove(int from, int to) {
    return Move((f << 14) + (to << 6) + from);
}

constexpr Move makeMove(int from, int to) {
    return Move((to << 6) + from);
}

constexpr int getFrom(Move move) {
    return move & 0x3f;
}

constexpr int getTo(Move move) {
    return (move >> 6) & 0x3f; 
}

constexpr FLAGS getFlag(Move move) {
    return FLAGS((move >> 14) & 0x3);
}

constexpr PieceType getProm(Move move) {
    return PieceType(((move >> 12) & 0x3) + 2);
}

constexpr int MAXMOVES = 256;

constexpr Piece makePiece(Colour c, PieceType pt) {
    return Piece((c << 3) + pt);
}

inline Colour colourOf(Piece pc) {
    return Colour(pc >> 3);
}

inline PieceType typeOf(Piece pc) {
    return PieceType(pc & 7);
}

template<Direction dir>
constexpr bitBoard shift(bitBoard bb) {
    return dir == NORTH ? bb << 8 : dir == NORTHEAST ? bb << 9
        :  dir == EAST ? bb << 1 : dir == SOUTHEAST ? bb >> 7
        :  dir == SOUTH ? bb >> 8 : dir == SOUTHWEST ? bb >> 9
        :  dir == WEST ? bb >> 1 : dir == NORTHWEST ? bb << 7
        : 0;
}


#endif