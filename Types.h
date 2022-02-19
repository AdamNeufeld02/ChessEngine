#ifndef TYPES_H
#define TYPES_H

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

constexpr int castlingRights[64] = {
    ~WHITE_OOO, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ~WHITE_CASTLING, ANY_CASTLING, ANY_CASTLING, ~WHITE_OO,
    ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING,
    ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING,
    ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING,
    ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING,
    ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING,
    ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING,
    ~BLACK_OOO, ANY_CASTLING, ANY_CASTLING, ANY_CASTLING, ~BLACK_CASTLING, ANY_CASTLING, ANY_CASTLING, ~BLACK_OO,
};

enum GenType {
    Legal, PseudoLegal, Evasions, Captures
};

// representing an encoded move
// bits (0-5) From
// bits (6-11) to
// bits (12- 13) promoted Piece code - 2 (KNIGHT = 0, BISHOP = 1, etc)
// bits(14- 15) flags
enum Move : short {
    NOMOVE
};

struct ScoredMove {
    Move move;
    int score;

    constexpr void operator = (Move m) {
        move = m;
    }
};

enum Value : int{
    Draw = -1,
    Infinity = 20000,
    NoValue = 20001
};

struct Score {
    short mg;
    short eg;

    Score() {
        mg = 0;
        eg = 0;
    }

    Score(int mg_, int eg_) {
        mg = mg_;
        eg = eg_;
    }

    Score operator +=(Score s){
        mg += s.mg;
        eg += s.eg;
        return *this;
    }

    Score operator -=(Score s) {
        mg -= s.mg;
        eg -= s.eg;
        return *this;
    }

    Score operator + (Score s) {
        s.mg += mg;
        s.eg += eg;
        return s;
    }

    Score operator - (Score s) {
        return Score(mg - s.mg, eg - s.eg);
    }

    Score operator +=(int cnst) {
        mg += cnst;
        eg += cnst;
        return *this;
    }
};

constexpr bool operator < (ScoredMove sm1, ScoredMove sm2) {
    return sm1.score < sm2.score;
}

constexpr bool operator == (ScoredMove sm1, ScoredMove sm2) {
    return sm1.score == sm2.score;
}

constexpr bool operator <= (ScoredMove sm1, ScoredMove sm2) {
    return sm1.score <= sm2.score;
}

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

#endif