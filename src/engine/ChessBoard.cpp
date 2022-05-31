#include "ChessBoard.h"

//Builds a chess Board from a given fenstring
ChessBoard::ChessBoard(std::string fenString, StateInfo& si) {
    fenToBoard(fenString, si);
}

ChessBoard::ChessBoard() {
    
}

void ChessBoard::copy(ChessBoard& cb) {
    colToMove = cb.colToMove;
    allPieces = cb.allPieces;
    st = cb.st;
    for (int i = 0; i < 64; i++) {
        board[i] = cb.board[i];
    }
    for (int i = 0; i < PIECENB; i++) {
        piecesByType[i] = cb.piecesByType[i];
    }
    for (int i = 0; i < 2; i++) {
        piecesByColour[i] = cb.piecesByColour[i];
        material[i] = cb.material[i];
        psqtv[i] = cb.psqtv[i];
    }
}

void ChessBoard::doMove(Move move, StateInfo& si) {
    FLAGS flag = getFlag(move);
    int from = getFrom(move);
    int to = getTo(move);
    
    Colour us = colToMove;
    Colour them = ~colToMove;
    Piece moved = pieceOn(from);
    Piece capt = flag == ENPASSENT? makePiece(them, PAWN) : pieceOn(to);

    memcpy(&si, st, offsetof(StateInfo, captured));
    si.pliesFromNull += 1;
    si.rule50 += 1;
    si.captured = capt;
    if (capt) {
        int capsq = to;
        if (flag == ENPASSENT) {
            capsq = us == WHITE ? to - 8 : to + 8;
        }
        si.key ^= Zobrist::psq[pieceOn(capsq)][capsq];
        if (typeOf(capt) == PAWN) {
            si.pawnKey ^= Zobrist::psq[pieceOn(capsq)][capsq];
        }
        removePiece(capsq);
        si.rule50 = 0;
    }
    if (flag == PROMOTION) {
        si.key ^= Zobrist::psq[pieceOn(from)][from];
        si.pawnKey ^= Zobrist::psq[pieceOn(from)][from];
        removePiece(from);
        putPiece(makePiece(us, getProm(move)), from);
    }
    if (flag == CASTLE) {
        if (to > from) {
            si.key ^= Zobrist::psq[pieceOn(to + 1)][to + 1];
            movePiece(to + 1, to - 1);
            si.key ^= Zobrist::psq[pieceOn(to - 1)][to - 1];
        } else {
            si.key ^= Zobrist::psq[pieceOn(to - 2)][to - 2];
            movePiece(to - 2, to + 1);
            si.key ^= Zobrist::psq[pieceOn(to + 1)][to + 1];
        }
    }
    if (si.epSquare > 0) {
        si.key ^= Zobrist::epSquare[(unsigned) si.epSquare];
    }
    si.epSquare = -1;
    if (typeOf(moved) == PAWN) {
        if ((to ^ from) == 16) {
            si.epSquare = us == WHITE ? from + 8 : from - 8;
            si.key ^= Zobrist::epSquare[(unsigned) si.epSquare];
        }

        if (flag != PROMOTION) {
            si.pawnKey ^= Zobrist::psq[pieceOn(from)][from];
            si.pawnKey ^= Zobrist::psq[pieceOn(from)][to];
        }
        si.rule50 = 0;
    }

    if (typeOf(moved) == KING) {
        si.pawnKey ^= Zobrist::psq[pieceOn(from)][from];
        si.pawnKey ^= Zobrist::psq[pieceOn(from)][to];
    }

    si.key ^= Zobrist::psq[pieceOn(from)][from];
    movePiece(from, to);
    si.key ^= Zobrist::psq[pieceOn(to)][to];

    si.previous = st;
    st = &si;
    colToMove = ~colToMove;

    st->key ^= Zobrist::colToMove;
    st->key ^= Zobrist::castlingKeys[st->castlingRights];
    st->castlingRights &= castlingRights[from] & castlingRights[to];
    st->key ^= Zobrist::castlingKeys[st->castlingRights];

    st->ply++;

    updateChecksAndPins(colToMove);

    st->repetitions = 0;
    int end = std::min(st->rule50, st->pliesFromNull);
    if (end >= 4)
    {
        StateInfo* stp = st->previous->previous;
        for (int i = 4; i <= end; i += 2)
        {
            stp = stp->previous->previous;
            if (stp->key == st->key)
            {
                st->repetitions = stp->repetitions + 1;
                break;
            }
        }
    }
}

void ChessBoard::undoMove(Move move) {
    FLAGS flag = getFlag(move);
    int from = getFrom(move);
    int to = getTo(move);
    colToMove = ~colToMove;
    Piece capt = st->captured;
    st = st->previous;
    movePiece(to, from);
    if (capt) {
        int captSq = to;
        if (flag == ENPASSENT) {
            captSq = colToMove == WHITE ? to - 8 : to + 8;
        }
        putPiece(capt, captSq);
    }
    if (flag == PROMOTION) {
        removePiece(from);
        putPiece(makePiece(colToMove, PAWN), from);
    }
    if (flag == CASTLE) {
        if (to > from) {
            movePiece(to - 1, to + 1);
        } else {
            movePiece(to + 1, to - 2);
        }
    }
}

void ChessBoard::doNullMove(StateInfo& si) {
    memcpy(&si, st, offsetof(StateInfo, captured));
    si.key ^= Zobrist::colToMove;
    si.captured = EMPTY;
    if (si.epSquare != -1) {
        si.key ^= Zobrist::epSquare[(unsigned) si.epSquare];
        si.epSquare = -1;
    }
    colToMove = ~colToMove;
    si.previous = st;
    si.pliesFromNull = 0;
    si.repetitions = 0;
    st = &si;
    updateChecksAndPins(colToMove);
    
}

void ChessBoard::undoNullMove() {
    colToMove = ~colToMove;
    st = st->previous;
}

zobristKey ChessBoard::keyAfter(Move move) {
    int from = getFrom(move);
    int to = getTo(move);
    Piece pc = pieceOn(from);
    Piece captured = pieceOn(to);
    zobristKey k = st->key ^ Zobrist::colToMove;

    if (captured)
        k ^= Zobrist::psq[captured][to];

    return k ^ Zobrist::psq[pc][to] ^ Zobrist::psq[pc][from];
}

void ChessBoard::updateChecksAndPins(Colour toMove) {
    st->checkersBB = getAttackers(getLSBIndex(pieces(toMove, KING)), ~toMove);
    st->pinnedBB[toMove] = blockersForSq(getLSBIndex(pieces(toMove, KING)), toMove, st->pinnersBB[~toMove]);
    st->pinnedBB[~toMove] = blockersForSq(getLSBIndex(pieces(~toMove, KING)), ~toMove, st->pinnersBB[toMove]);
}

bitBoard ChessBoard::blockersForSq(int sq, Colour blockingCol, bitBoard& pinners) {
    bitBoard blockers = 0;
    pinners = 0;

    bitBoard snipers = (genAttacksBB<ROOK>(sq) & (pieces(~blockingCol, ROOK) | pieces(~blockingCol, QUEEN))) |
                       (genAttacksBB<BISHOP>(sq) & (pieces(~blockingCol, BISHOP) | pieces(~blockingCol, QUEEN)));

    bitBoard occupancy = pieces() ^ snipers;

    while (snipers)
    {
        int sniperSq = getLSBIndex(snipers);
        bitBoard between = getBetweenBB(sq, sniperSq);
        bitBoard pinned = between & occupancy;
        bitBoard pinner = between & squares[sniperSq];
        if (pinned && !moreThanOne(pinned)) {
            blockers |= pinned;
            if (pinned & pieces(blockingCol)) {
                pinners |= pinner;
            }
        }
        snipers ^= pinner;
    }
    return blockers;
}

bitBoard ChessBoard::getAttackers(int sq, Colour c) {
    bitBoard attackers = genAttacksBB<ROOK>(sq, allPieces) & (pieces(c, ROOK) | pieces(c, QUEEN));
    attackers |= genAttacksBB<BISHOP>(sq, allPieces) & (pieces(c, BISHOP) | pieces(c, QUEEN));
    attackers |= genAttacksBB<KNIGHT>(sq) & pieces(c, KNIGHT);
    attackers |= genAttacksBB<KING>(sq) & pieces(c, KING);
    attackers |= pawnAttacks[~c][sq] & pieces(c, PAWN);
    return attackers;
}

bitBoard ChessBoard::attackersForOcc(int sq, bitBoard occ) {
    return (pawnAttacks[WHITE][sq] & pieces(WHITE, PAWN)) |
           (pawnAttacks[BLACK][sq] & pieces(BLACK, PAWN)) |
           (genAttacksBB<ROOK>(sq, occ) & (pieces(ROOK) | pieces(QUEEN))) |
           (genAttacksBB<BISHOP>(sq, occ) & (pieces(BISHOP) | pieces(QUEEN))) |
           (genAttacksBB<KNIGHT>(sq) & pieces(KNIGHT)) |
           (genAttacksBB<KING>(sq) & pieces(KING)); 
}

bitBoard ChessBoard::getSlidingAttacks(bitBoard occ) {
    Colour attackingCol = ~colourToMove();
    bitBoard rooks = pieces(attackingCol, ROOK);
    bitBoard bishops = pieces(attackingCol, BISHOP);
    bitBoard queens = pieces(attackingCol, QUEEN);
    bitBoard slidingAttacks = 0;
    while (rooks) {
        slidingAttacks |= genAttacksBB<ROOK>(popLSB(rooks), occ);
    }
    while (bishops) {
        slidingAttacks |= genAttacksBB<BISHOP>(popLSB(bishops), occ);
    } 
    while (queens) {
        slidingAttacks |= genAttacksBB<QUEEN>(popLSB(queens), occ);
    }
    return slidingAttacks;
}

bool ChessBoard::seeGE(Move move, int threshold) {
    // Only deal with normal moves
    if (getFlag(move) != NOFLAG) return true;

    int from = getFrom(move);
    int to = getTo(move);

    int swap = pieceVals[typeOf(pieceOn(to))].mg - threshold;
    if (swap < 0) return false;

    swap = pieceVals[typeOf(pieceOn(from))].mg - swap;
    if (swap <= 0) return true;

    bitBoard occ = pieces() ^ squares[from] ^ squares[to];
    bitBoard attackers = attackersForOcc(to, occ);
    Colour stm = colToMove;
    bitBoard stmAttackers, bb;
    int res = 1;
    while (true) {
        stm = ~stm;
        attackers &= occ;

        // If the side to move has no more attackers we are finished
        if (!(stmAttackers = attackers & pieces(stm))) break;

        // Don't allow pinned pieces to attack
        if (pinners(~stm) & occ) {
            stmAttackers &= ~pinned(stm);
        }

        if (!stmAttackers) break;

        res ^= 1;

        // Get the least valuable attacker
        if ((bb = stmAttackers & pieces(stm, PAWN))) {
            if ((swap = pieceVals[PAWN].mg - swap) < res) break;

            occ ^= squares[getLSBIndex(bb)];
            attackers |= genAttacksBB<BISHOP>(to, occ) & (pieces(BISHOP) | pieces(QUEEN));
        } else if ((bb = stmAttackers & pieces(stm, KNIGHT))) {
            if ((swap = pieceVals[KNIGHT].mg - swap) < res) break;

            occ ^= squares[getLSBIndex(bb)];
        } else if ((bb = stmAttackers & pieces(stm, BISHOP))) {
            if ((swap = pieceVals[BISHOP].mg - swap) < res) break;

            occ ^= squares[getLSBIndex(bb)];
            attackers |= genAttacksBB<BISHOP>(to, occ) & (pieces(BISHOP) | pieces(QUEEN));
        } else if ((bb = stmAttackers & pieces(stm, ROOK))) {
            if ((swap = pieceVals[ROOK].mg - swap) < res) break;

            occ ^= squares[getLSBIndex(bb)];
            attackers |= genAttacksBB<ROOK>(to, occ) & (pieces(ROOK) | pieces(QUEEN));
        } else if ((bb = stmAttackers & pieces(stm, QUEEN))) {
            if ((swap = pieceVals[QUEEN].mg - swap) < res) break;

            occ ^= squares[getLSBIndex(bb)];
            attackers |= genAttacksBB<ROOK>(to, occ) & (pieces(ROOK) | pieces(QUEEN));
            attackers |= genAttacksBB<BISHOP>(to, occ) & (pieces(BISHOP) | pieces(QUEEN));
        } else {
            return (attackers & ~pieces(stm)) ? res ^ 1 : res;
        }
    }

    return res;
    
}

void ChessBoard::printBoard(bitBoard bb) {
    std::bitset<64> bits = std::bitset<64>(bb);
    int file;
    int rank;
    for (rank = 7; rank >= 0; rank--) {
        for (file = 0; file < 8; file++) {
            std::cout << bits[rank * 8 + file] << " ";
        }
        std::cout << std::endl;
    }
}

void ChessBoard::fenToBoard(std::string fenString, StateInfo& si) {
    initBoard(si);
    int boardIndex = 56;
    int stringIndex = 0;
    char curr = fenString[0];
    int num;
    while (curr != *" ") {
        if (curr == *"/") {
            stringIndex++;
            boardIndex -= 16;
            curr = fenString[stringIndex];
            continue;
        }
        char* endPtr = &curr;
        strtol(&curr, &endPtr, 10);
        if (endPtr != &curr) {
            num = curr - 48;
            if (num >= 1) {
                boardIndex += num;
            }
        } else { 
            switch(curr) {
                case(*"P"): putPiece(WHITEPAWN, boardIndex);
                    st->key ^= Zobrist::psq[WHITEPAWN][boardIndex];
                    st->pawnKey ^= Zobrist::psq[WHITEPAWN][boardIndex];
                    break;
                case(*"N"): putPiece(WHITEKNIGHT, boardIndex);
                    st->key ^= Zobrist::psq[WHITEKNIGHT][boardIndex];
                    break;
                case(*"R"): putPiece(WHITEROOK, boardIndex);
                    st->key ^= Zobrist::psq[WHITEROOK][boardIndex];
                    break;
                case(*"B"): putPiece(WHITEBISHOP, boardIndex);
                    st->key ^= Zobrist::psq[WHITEBISHOP][boardIndex];
                    break;
                case(*"Q"): putPiece(WHITEQUEEN, boardIndex);
                    st->key ^= Zobrist::psq[WHITEQUEEN][boardIndex];
                    break;
                case(*"K"): putPiece(WHITEKING, boardIndex);
                    st->key ^= Zobrist::psq[WHITEKING][boardIndex];
                    st->pawnKey ^= Zobrist::psq[WHITEKING][boardIndex];
                    break;
                case(*"p"): putPiece(BLACKPAWN, boardIndex);
                    st->key ^= Zobrist::psq[BLACKPAWN][boardIndex];
                    st->pawnKey ^= Zobrist::psq[BLACKPAWN][boardIndex];
                    break;
                case(*"n"): putPiece(BLACKKNIGHT, boardIndex);
                    st->key ^= Zobrist::psq[BLACKKNIGHT][boardIndex];
                    break;
                case(*"r"): putPiece(BLACKROOK, boardIndex);
                    st->key ^= Zobrist::psq[BLACKROOK][boardIndex];
                    break;
                case(*"b"): putPiece(BLACKBISHOP, boardIndex);
                    st->key ^= Zobrist::psq[BLACKBISHOP][boardIndex];
                    break;
                case(*"q"): putPiece(BLACKQUEEN, boardIndex);
                    st->key ^= Zobrist::psq[BLACKQUEEN][boardIndex];
                    break;
                case(*"k"): putPiece(BLACKKING, boardIndex);
                    st->key ^= Zobrist::psq[BLACKKING][boardIndex];
                    st->pawnKey ^= Zobrist::psq[BLACKKING][boardIndex];
                    break;
            }
            boardIndex++;
        }
        stringIndex++;
        curr = fenString[stringIndex];
    }

    // Parse next Move
    stringIndex++;
    curr = fenString[stringIndex];
    if (curr == *"w") {
        colToMove = WHITE;
    } else {
        colToMove = BLACK;
        st->key ^= Zobrist::colToMove;
    }

    // Parse Castling Rights
    stringIndex += 2;
    curr = fenString[stringIndex];
    while (curr != *" ") {
        if (curr == *"K") {
            st->castlingRights |= WHITE_OO;
        } else if (curr == *"Q") {
            st->castlingRights |= WHITE_OOO;
        } else if (curr == *"k") {
            st->castlingRights |= BLACK_OO;
        } else if (curr == *"q") {
            st->castlingRights |= BLACK_OOO;
        }
        stringIndex++;
        curr = fenString[stringIndex];
    }
    st->key ^= Zobrist::castlingKeys[st->castlingRights];

    stringIndex++;
    curr = fenString[stringIndex];

    // parse En Passent square
    if (curr != *"-") {
        if (curr == *"a") {
            st->epSquare = 0;
        } else if (curr == *"b") {
            st->epSquare = 1;
        } else if (curr == *"c") {
            st->epSquare = 2;
        } else if (curr == *"d") {
            st->epSquare = 3;
        } else if (curr == *"e") {
            st->epSquare = 4;
        } else if (curr == *"f") {
            st->epSquare = 5;
        } else if (curr == *"g") {
            st->epSquare = 6;
        } else if (curr == *"h") {
            st->epSquare = 7;
        }
        stringIndex++;
        curr = fenString[stringIndex];
        num = strtol(&curr, NULL, 10);
        st->epSquare += (8 - num) * 8;
        st->key ^= Zobrist::epSquare[(unsigned) st->epSquare];
    } else {
        st->epSquare = -1;
    }
    updateChecksAndPins(colToMove);
}

std::string ChessBoard::boardToFen() {
    static std::string pieceMap[PIECENB] = {"\0", "P", "N", "B", "R", "Q", "K", "\0", "\0", "p", "n", "b", "r", "q", "k"};
    static std::string numMap[9] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};
    std::string ret;
    std::string curr;
    int empty = 0;
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            curr = pieceMap[pieceOn(rank * 8 + file)];
            if (curr != "\0") {
                if (empty) {
                    ret.append(numMap[empty]);
                    empty = 0;
                }
                ret.append(curr);
            } else {
                empty++;
            }
        }
        if (empty) {
            ret.append(numMap[empty]);
            empty = 0;
        }
        if (rank > 0) {
            ret.append("/");
        } else {
            ret.append(" ");
        }
    }

    if (colToMove == WHITE) {
        ret.append("w ");
    } else {
        ret.append("b ");
    }

    if (st->castlingRights) {
        if (canCastle(WHITE_OO)) {
            ret.append("K");
        }
        if (canCastle(WHITE_OOO)) {
            ret.append("Q");
        }
        if (canCastle(BLACK_OO)) {
            ret.append("k");
        }
        if (canCastle(BLACK_OOO)) {
            ret.append("q");
        }
    } else {
        ret.append("-");
    }

    ret.append(" ");

    if (st->epSquare > 0) {
        ret.append(Misc::indexToString(st->epSquare));
    } else {
        ret.append("-");
    }
    return ret;
}

// Initializes all bitboards to empty and all fields
void ChessBoard::initBoard(StateInfo& si) {
    for (int i = 0; i < PIECENB; i++) {
        piecesByType[i] = (bitBoard)0;
    }
    for (int i = 0; i < 64; i++) {
        board[i] = EMPTY;
    }
    piecesByColour[WHITE] = (bitBoard)0;
    piecesByColour[BLACK] = (bitBoard)0;
    allPieces = (bitBoard)0;

    st = &si;

    st->key = 0;
    st->ply = 0;
    st->pawnKey = 0;
    st->castlingRights = NO_CASTLING;
    st->epSquare = -1;
    st->previous = NULL;
    st->checkersBB = 0;
    st->pinnedBB[WHITE] = 0;
    st->pinnedBB[BLACK] = 0;
    st->pinnersBB[WHITE] = 0;
    st->pinnersBB[BLACK] = 0;
    st->captured = EMPTY;
    st->rule50 = 0;
    st->pliesFromNull = 0;
    st->repetitions = 0;
    material[WHITE] = Score(0, 0);
    material[BLACK] = Score(0, 0);
    psqtv[WHITE] = Score(0, 0);
    psqtv[BLACK] = Score(0, 0);
    colToMove = WHITE;
    thisThread = NULL;
}

