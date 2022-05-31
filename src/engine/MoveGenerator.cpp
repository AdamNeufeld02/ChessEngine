#include "MoveGenerator.h"

// Generates legal moves for a given chess position. If onlyCaptures is specified then only legal captures are generated
// unless the player to move is in check, then all legal moves will be generated.
ScoredMove* MoveGenerator::generateMoves(ChessBoard& chessBoard, ScoredMove* moves, bool onlyCaptures) {
    ScoredMove* curr = moves;
    Colour us = chessBoard.colourToMove();
    int ksq = getLSBIndex(chessBoard.pieces(us, KING));
    bitBoard pinned = chessBoard.pinned(us) & chessBoard.pieces(us);
    moves = chessBoard.checkers() ? generateAllMoves<Evasions>(chessBoard, moves) : onlyCaptures? generateAllMoves<Captures>(chessBoard, moves) : generateAllMoves<Legal>(chessBoard, moves);
    if (pinned) {
        while (curr != moves) {
            if ((pinned & squares[getFrom(curr->move)]) && !isAligned(getFrom(curr->move), getTo(curr->move), ksq)) {
                *curr = *--moves;
            } else {
                ++curr;
            }
        } 
    }
    return moves;
}

template<GenType t>
ScoredMove* MoveGenerator::generateAllMoves(ChessBoard& chessBoard, ScoredMove* moves) {
    Colour us = chessBoard.colourToMove();
    return us == WHITE? generateMoves<t, WHITE>(chessBoard, moves) : generateMoves<t, BLACK>(chessBoard, moves);
}

template<GenType t, Colour us>
ScoredMove* MoveGenerator::generateMoves(ChessBoard& chessBoard, ScoredMove* moves) {
    bitBoard targets;
    int ksq = getLSBIndex(chessBoard.pieces(us, KING));
    if (t != Evasions || !moreThanOne(chessBoard.checkers())) {
        targets = t == Legal ? ~chessBoard.pieces(us) : t == Evasions ? getBetweenBB(ksq, getLSBIndex(chessBoard.checkers())) :
                  t == Captures ? chessBoard.pieces(~ us) : ~chessBoard.pieces();
        moves = generateMoves<ROOK, us>(chessBoard, moves, targets);
        moves = generateMoves<BISHOP, us>(chessBoard, moves, targets);
        moves = generateMoves<QUEEN, us>(chessBoard, moves, targets);
        moves = generateMoves<KNIGHT, us>(chessBoard, moves, targets);
        moves = generatePawnMoves<t, us>(chessBoard, moves, targets);
    }
    
    moves = generateKingMoves<t, us>(chessBoard, moves);
    return moves;
}

template<PieceType pt, Colour us>
ScoredMove* MoveGenerator::generateMoves(ChessBoard& chessBoard, ScoredMove* moves, bitBoard targets) {
    int from, to;
    bitBoard attacks;
    bitBoard pieces = chessBoard.pieces(us, pt);
    bitBoard occ = chessBoard.pieces();
    while (pieces) {
        from = popLSB(pieces);
        attacks = genAttacksBB<pt>(from, occ) & targets;
        while (attacks) {
            to = popLSB(attacks);
            *moves++ = makeMove(from, to);
        }
    }
    return moves;
}

template<GenType t, Colour us>
ScoredMove* MoveGenerator::generateKingMoves(ChessBoard& chessBoard, ScoredMove* moves) {
    bitBoard attacks;
    bitBoard pieces = chessBoard.pieces(us, KING);
    bitBoard targets = t == Captures ? chessBoard.pieces(~us) : ~chessBoard.pieces(us);
    int from, to;
    CastlingRights kingCastle;
    CastlingRights queenCastle;
    // init colour specific castling rights
    if (us == WHITE) {
        kingCastle = WHITE_OO;
        queenCastle = WHITE_OOO;
    } else {
        kingCastle = BLACK_OO;
        queenCastle = BLACK_OOO;
    }

    // lookup precomputed king moves
    while(pieces) {
        from = popLSB(pieces);
        attacks = genAttacksBB<KING>(from) & targets;
        if (attacks) {
            attacks &= ~(chessBoard.getSlidingAttacks(chessBoard.pieces() & ~chessBoard.pieces(us, KING)));
            bitBoard enemyknights = chessBoard.pieces(~us, KNIGHT);
            bitBoard enemeyPawns = chessBoard.pieces(~us, PAWN);
            bitBoard enemeyKing = chessBoard.pieces(~us, KING);
            while (attacks) {
                to = popLSB(attacks);
                if (!(pawnAttacks[us][to] & enemeyPawns) && !(knightAttacks[to] & enemyknights) && !(kingAttacks[to] & enemeyKing)) {
                    *moves++ = makeMove(from, to);
                }
            }
        }
        // Generate Castles
        if (t == Legal && (castleMasks[kingCastle] & chessBoard.pieces()) == 0 && chessBoard.canCastle(kingCastle)) {
            if (!chessBoard.getAttackers(from + 1, ~us) && !chessBoard.getAttackers(from + 2, ~us)) {
                *moves++ = makeMove<CASTLE>(from, from + 2);
            }
        }

        if (t == Legal && (castleMasks[queenCastle] & chessBoard.pieces()) == 0 && chessBoard.canCastle(queenCastle)) {
            if (!chessBoard.getAttackers(from - 1, ~us) && !chessBoard.getAttackers(from - 2, ~us)) {
                 *moves++ = makeMove<CASTLE>(from, from - 2);
            }
        }
    }
    return moves;
}

// Generates all pawn moves including promotions double pushes and enpassents
template<GenType t, Colour us>
ScoredMove* MoveGenerator::generatePawnMoves(ChessBoard& chessBoard, ScoredMove* moves, bitBoard targets) {
    Colour them = ~us;
    constexpr Direction upRight = us == WHITE ? NORTHEAST : SOUTHEAST;
    constexpr Direction upLeft = us == WHITE ? NORTHWEST : SOUTHWEST;
    constexpr Direction up = us == WHITE? NORTH : SOUTH;
    bitBoard pawnsNotOn7 = chessBoard.pieces(us, PAWN);
    bitBoard pawnsOn7 = us == WHITE? pawnsNotOn7 & Rank7BB : pawnsNotOn7 & Rank2BB;
    pawnsNotOn7 ^= pawnsOn7;
    bitBoard rank3 = us == WHITE? Rank3BB : Rank6BB;

    bitBoard empty = ~chessBoard.pieces();
    bitBoard enemies = t == Evasions? chessBoard.checkers() : chessBoard.pieces(them);
    
    // gen pushes if type is not captures
    if (t != Captures) {
        bitBoard b1 = shift<up>(pawnsNotOn7) & empty;
        bitBoard b2 = shift<up>(b1 & rank3) & empty;
        if (t == Evasions) {
            b1 &= targets;
            b2 &= targets;
        }
        while (b1) {
            int to = popLSB(b1);
            *moves++ = makeMove(to - up, to);
        }

        while (b2) {
            int to = popLSB(b2);
            *moves++ = makeMove(to - up - up, to);
        }
    }

    // gen promotions (push promotions even if gentype is captures only)
    if (pawnsOn7) {
        bitBoard b1 = shift<upRight>(pawnsOn7 & ~FileHBB)  & enemies; 
        bitBoard b2 = shift<upLeft>(pawnsOn7 & ~FileABB)  & enemies;
        bitBoard b3 = shift<up>(pawnsOn7) & empty;
        if (t == Evasions) {
            b3 &= targets;
        }
        while (b1) {
            int to = popLSB(b1);
            for (int i = KNIGHT; i < KING; i++) {
                *moves++ = makeMove(to - upRight, to, PieceType(i));
            }
        }
        while (b2) {
            int to = popLSB(b2);
            for (int i = KNIGHT; i < KING; i++) {
                *moves++ = makeMove(to - upLeft, to, PieceType(i));
            }
        }
        while (b3) {
            int to = popLSB(b3);
            for (int i = KNIGHT; i < KING; i++) {
                *moves++ = makeMove(to - up, to, PieceType(i));
            }
        }
    }

    bitBoard b1 = shift<upRight>(pawnsNotOn7 & ~FileHBB) & enemies;
    bitBoard b2 = shift<upLeft>(pawnsNotOn7 & ~FileABB) & enemies;

    while (b1) {
        int to = popLSB(b1);
        *moves++ = makeMove(to - upRight, to);
    }

    while (b2) {
        int to = popLSB(b2);
        *moves++ = makeMove(to - upLeft, to);
    }

    if (chessBoard.epSquare() > 0) {
        int epSquare = chessBoard.epSquare();
        bitBoard b1 = pawnAttacks[them][epSquare] & pawnsNotOn7;
        int ksq = getLSBIndex(chessBoard.pieces(us, KING));
        while (b1) {
            int from = popLSB(b1);
            bitBoard capturedOcc = (chessBoard.pieces() ^ squares[epSquare - up] ^ squares[from]) | squares[epSquare];
            bitBoard lineAttack = genAttacksBB<ROOK>(ksq, capturedOcc) & (chessBoard.pieces(them, ROOK) | chessBoard.pieces(them, QUEEN));
            bitBoard diagAttack = genAttacksBB<BISHOP>(ksq, capturedOcc) & (chessBoard.pieces(them, BISHOP) | chessBoard.pieces(them, QUEEN));
            // if enpassent exposes king dont do it
            if (!(lineAttack || diagAttack)) {
                *moves++ = makeMove<ENPASSENT>(from, epSquare);
            }
        }
    }
    return moves;
}