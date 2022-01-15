#include "MoveGenerator.h"

constexpr bitBoard MoveGenerator::bishopMagicNums[64];
constexpr bitBoard MoveGenerator::rookMagicNums[64];

MoveGenerator::MoveGenerator() {
    initCastleMasks();
    precomputeAttackSets();
}

Move* MoveGenerator::generateMoves(ChessBoard* chessBoard, Move* moves) {
    if (chessBoard->colourToMove() == WHITE) {
        return generateMoves<Legal, WHITE>(*chessBoard, moves);
    }
    return generateMoves<Legal, BLACK>(*chessBoard, moves);
}

template<GenType t, Colour c>
Move* MoveGenerator::generateMoves(ChessBoard& chessBoard, Move* moves) {
    bitBoard targets = ~chessBoard.pieces(c);
    moves = generateMoves<ROOK>(chessBoard, moves, chessBoard.pieces(c, ROOK), targets);
    moves = generateMoves<BISHOP>(chessBoard, moves, chessBoard.pieces(c, BISHOP), targets);
    moves = generateMoves<QUEEN>(chessBoard, moves, chessBoard.pieces(c, QUEEN), targets);
    moves = generateMoves<KNIGHT>(chessBoard, moves, chessBoard.pieces(c, KNIGHT), targets);
    moves = generateKingMoves(chessBoard, moves, chessBoard.pieces(c, KING), targets);
    moves = generatePawnMoves<t, c>(chessBoard, moves);
    return moves;
}

template<PieceType pt>
Move* MoveGenerator::generateMoves(ChessBoard& chessBoard, Move* moves, bitBoard pieces, bitBoard targets) {
    int from, to;
    bitBoard attacks;
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

Move* MoveGenerator::generateKingMoves(ChessBoard& chessBoard, Move* moves, bitBoard pieces, bitBoard targets) {
    bitBoard attacks;
    int from, to;
    CastlingRights kingCastle;
    CastlingRights queenCastle;
    // init colour specific castling rights
    if (chessBoard.colourToMove() == WHITE) {
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
        while (attacks) {
            to = popLSB(attacks);
            *moves++ = makeMove(from, to);
        }

        // Generate Castles
        if ((castleMasks[kingCastle] & chessBoard.pieces()) == 0 && chessBoard.canCastle(kingCastle)) {
            *moves++ = makeMove<CASTLE>(from, from + 2);
        }

        if ((castleMasks[queenCastle] & chessBoard.pieces()) == 0 && chessBoard.canCastle(queenCastle)) {
            *moves++ = makeMove<CASTLE>(from, from - 2);
        }
    }
    return moves;
}

template<GenType t, Colour us>
Move* MoveGenerator::generatePawnMoves(ChessBoard& chessBoard, Move* moves) {
    Colour them = ~us;
    constexpr Direction upRight = us == WHITE ? NORTHEAST : SOUTHWEST;
    constexpr Direction upLeft = us == WHITE ? NORTHWEST : SOUTHEAST;
    constexpr Direction up = us == WHITE? NORTH : SOUTH;
    bitBoard pawnsNotOn7 = chessBoard.pieces(us, PAWN);
    bitBoard pawnsOn7 = us == WHITE? pawnsNotOn7 & Rank7BB : pawnsNotOn7 & Rank2BB;
    pawnsNotOn7 ^= pawnsOn7;
    bitBoard rank3 = us == WHITE? Rank3BB : Rank6BB;

    bitBoard empty = ~chessBoard.pieces();
    bitBoard enemies = chessBoard.pieces(them);
    
    // gen pushes if type is not captures
    if (t != Captures) {
        bitBoard b1 = shift<up>(pawnsNotOn7) & empty;
        bitBoard b2 = shift<up>(b1 & rank3) & empty;
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
        bitBoard b1 = shift<upRight>(pawnsOn7) & enemies;
        bitBoard b2 = shift<upLeft>(pawnsOn7) & enemies;
        bitBoard b3 = shift<up>(pawnsOn7) & empty;
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

    bitBoard b1 = shift<upRight>(pawnsNotOn7) & enemies;
    bitBoard b2 = shift<upLeft>(pawnsNotOn7) & enemies;

    while (b1) {
        int to = popLSB(b1);
        *moves++ = makeMove(to - upRight, to);
    }

    while (b2) {
        int to = popLSB(b2);
        *moves++ = makeMove(to - upLeft, to);
    }

    if (chessBoard.epSquare() > 0) {
        bitBoard b1 = pawnAttacks[them][(int)chessBoard.epSquare()] & pawnsNotOn7;
        while (b1) {
            int from = popLSB(b1);
            *moves++ = makeMove<ENPASSENT>(from, chessBoard.epSquare());
        }
    }
    return moves;
}

bitBoard MoveGenerator::pushUp(bitBoard board, Colour c) {
    if (c == WHITE) {
        return board << 8;
    } else {
        return board >> 8;
    }
}

bitBoard MoveGenerator::genOccMask(int square, int rook) {
    bitBoard occMask = 0;
    int file = square % 8;
    int rank = square / 8;
    if (rook) {
        for (int nfile = file + 1; nfile < 7; nfile++) ChessBoard::setBit(occMask, (8 * rank) + nfile);
        for (int nrank = rank + 1; nrank < 7; nrank++) ChessBoard::setBit(occMask, (8* nrank) + file);
        for (int nfile = file - 1; nfile > 0; nfile--) ChessBoard::setBit(occMask, (8 * rank) + nfile);
        for (int nrank = rank - 1; nrank > 0; nrank--) ChessBoard::setBit(occMask, (8* nrank) + file);
    } else {
        for (int nfile = file + 1, nrank = rank + 1; nfile < 7 && nrank < 7; nrank++, nfile++)
            ChessBoard::setBit(occMask, (8 * nrank) + nfile);
        for (int nfile = file + 1, nrank = rank - 1; nfile < 7 && nrank > 0; nrank--, nfile++)
            ChessBoard::setBit(occMask, (8 * nrank) + nfile);
        for (int nfile = file - 1, nrank = rank - 1; nfile > 0 && nrank > 0; nrank--, nfile--)
            ChessBoard::setBit(occMask, (8 * nrank) + nfile);
        for (int nfile = file - 1, nrank = rank + 1; nfile > 0 && nrank < 7; nrank++, nfile--)
            ChessBoard::setBit(occMask, (8 * nrank) + nfile);
    }
    return occMask;
}

bitBoard MoveGenerator::setOccupancy(int index, int bitsInMask, bitBoard attackMask) {
    bitBoard occ = 0;

    for (int count = 0; count < bitsInMask; count++) {
        int bit = popLSB(attackMask);
        if (index & (1 << count)) {
            ChessBoard::setBit(occ, bit);
        }
    }

    return occ;
}

void MoveGenerator::initCastleMasks() {
    castleMasks[WHITE_OO] = (bitBoard)0x60;
    castleMasks[WHITE_OOO] = (bitBoard)0xe;
    castleMasks[BLACK_OO] = castleMasks[WHITE_OO] << 56;
    castleMasks[BLACK_OOO] = castleMasks[WHITE_OOO] << 56;
}

// Precompute attack tables and initialize magic bitboards
void MoveGenerator::precomputeAttackSets() {
    for (int i = 0; i < 64; i++) {
        pawnAttacks[0][i] = computePawnAttack(i, 1);
        pawnAttacks[1][i] = computePawnAttack(i, 0);
        knightAttacks[i] = computeKnightAttack(i);
        kingAttacks[i] = computeKingAttack(i);
    }
    initRookMagics();
    initBishopMagics();
}

void MoveGenerator::initRookMagics() {
    bitBoard occ;
    int size, bits;
    for (int i = 0; i < 64; i++) {
        MagicSquare& m = rookMagics[i];
        m.occMask = genOccMask(i, 1);
        bits = countBits(m.occMask);
        m.attacks = i == 0 ? rookAttacks : rookMagics[i - 1].attacks + size;
        m.shift = 64 - bits;
        m.magic = rookMagicNums[i];
        size = 1 << bits;
        for (int k = 0; k < size; k++) {
            occ = setOccupancy(k, bits, m.occMask);
            m.attacks[occ * m.magic >> m.shift] = computeRookAttack(i, occ);
        }
    }
}

void MoveGenerator::initBishopMagics() {
    bitBoard occ;
    int size, bits;
    for (int i = 0; i < 64; i++) {
        MagicSquare& m = bishopMagics[i];
        m.occMask = genOccMask(i, 0);
        bits = countBits(m.occMask);
        m.attacks = i == 0? bishopAttacks : bishopMagics[i - 1].attacks + size;
        m.shift = 64 - bits;
        m.magic = bishopMagicNums[i];
        size = 1 << bits;
        for (int k = 0; k < size; k++) {
            occ = setOccupancy(k, bits, m.occMask);
            m.attacks[occ * m.magic >> m.shift] = computeBishopAttack(i, occ);
        }
    } 
}

// computes the rook attack for a given occupancy. Goes through all valid rook directions (north east etc.) until a set bit
// is found in the occupancy board
bitBoard MoveGenerator::computeRookAttack(int square, bitBoard occ) {
    bitBoard attacks = 0;
    bitBoard curr;
    int file = square % 8;
    int rank = square / 8;
    for (int nrank = rank + 1; nrank < 8; nrank++) {
        curr = 0;
        ChessBoard::setBit(curr, (8 * nrank) + file);
        attacks |= curr;
        if (occ & curr) break;
    }
    for (int nfile = file + 1; nfile < 8; nfile++) {
        curr = 0;
        ChessBoard::setBit(curr, (8 * rank) + nfile);
        attacks |= curr;
        if (occ & curr) break;
    }
    for (int nrank = rank - 1; nrank >=  0; nrank--) {
        curr = 0;
        ChessBoard::setBit(curr, (8 * nrank) + file);
        attacks |= curr;
        if (occ & curr) break;
    }
    for (int nfile = file - 1; nfile >= 0; nfile--) {
        curr = 0;
        ChessBoard::setBit(curr, (8 * rank) + nfile);
        attacks |= curr;
        if (occ & curr) break;
    }
    return attacks;
}

// Computes Bishop Attack for an occupancy. Goes through all valid bishop directions (northWest northEast, etc) one by one until
// a set bit is found in the occupancy board.
bitBoard MoveGenerator::computeBishopAttack(int square, bitBoard occ) {
    bitBoard attacks = 0;
    int file = square % 8;
    int rank = square / 8;
    bitBoard curr;
    for (int nrank = rank + 1, nfile = file + 1; nrank < 8 && nfile < 8; nrank++, nfile++) {
        curr = 0;
        ChessBoard::setBit(curr, (8 * nrank) + nfile);
        attacks |= curr;
        if (occ & curr) break;
    }
    for (int nrank = rank + 1, nfile = file - 1; nrank < 8 && nfile >= 0; nrank++, nfile--) {
        curr = 0;
        ChessBoard::setBit(curr, (8 * nrank) + nfile);
        attacks |= curr;
        if (occ & curr) break;
    }
    for (int nrank = rank - 1, nfile = file - 1; nrank >= 0 && nfile >= 0; nrank--, nfile--) {
        curr = 0;
        ChessBoard::setBit(curr, (8 * nrank) + nfile);
        attacks |= curr;
        if (occ & curr) break;
    }
    for (int nrank = rank - 1, nfile = file + 1; nrank >=0  && nfile < 8; nrank--, nfile++) {
        curr = 0;
        ChessBoard::setBit(curr, (8 * nrank) + nfile);
        attacks |= curr;
        if (occ & curr) break;
    }
    return attacks;
}

bitBoard MoveGenerator::computeKnightAttack(int square) {
    bitBoard attacks = 0ULL;
    int file = square % 8;
    int rank = square / 8;
    int nrank, nfile;
    nrank = rank + 2;
    if (nrank < 8) {
        nfile = file + 1;
        if (nfile < 8) {
            ChessBoard::setBit(attacks, (nrank * 8) + nfile);
        }
        nfile = file - 1;
        if (nfile >= 0) {
            ChessBoard::setBit(attacks, (nrank * 8) + nfile);
        }
    }
    nfile = file + 2;
    if (nfile < 8) {
        nrank = rank + 1;
        if (nrank < 8) {
            ChessBoard::setBit(attacks, (nrank * 8) + nfile);
        }
        nrank = rank - 1;
        if (nrank >= 0) {
            ChessBoard::setBit(attacks, (nrank * 8) + nfile);
        }
    }
    nrank = rank - 2;
    if (nrank >= 0) {
        nfile = file + 1;
        if (nfile < 8) {
            ChessBoard::setBit(attacks, (nrank * 8) + nfile);
        }
        nfile = file - 1;
        if (nfile >= 0) {
            ChessBoard::setBit(attacks, (nrank * 8) + nfile);
        }
    }
    nfile = file - 2;
    if (nfile >= 0) {
        nrank = rank + 1;
        if (nrank < 8) {
            ChessBoard::setBit(attacks, (nrank * 8) + nfile);
        }
        nrank = rank - 1;
        if (nrank >= 0) {
            ChessBoard::setBit(attacks, (nrank * 8) + nfile);
        }
    }
    return attacks;
}

bitBoard MoveGenerator::computeKingAttack(int square) {
    bitBoard attacks = 0ULL;
    int file = square % 8;
    int rank = square / 8;
    int nrank, nfile;
    nrank = rank + 1;
    if (nrank < 8) {
        nfile = file - 1;
        if (nfile >= 0) {
            ChessBoard::setBit(attacks, (nrank * 8) + nfile);
        }
        nfile = file;
        ChessBoard::setBit(attacks, (nrank * 8) + nfile);
        nfile = file + 1;
        if (nfile < 8) {
            ChessBoard::setBit(attacks, (nrank * 8) + nfile);
        }
    }
    nrank = rank;
    nfile = file - 1;
    if (nfile >= 0) {
        ChessBoard::setBit(attacks, (nrank * 8) + nfile);
    }
    nfile = file + 1;
    if (nfile < 8) {
        ChessBoard::setBit(attacks, (nrank * 8) + nfile);
    }
    nrank = rank - 1;
    if (nrank >= 0) {
        nfile = file - 1;
        if (nfile >= 0) {
            ChessBoard::setBit(attacks, (nrank * 8) + nfile);
        }
        nfile = file;
        ChessBoard::setBit(attacks, (nrank * 8) + nfile);
        nfile = file + 1;
        if (nfile < 8) {
            ChessBoard::setBit(attacks, (nrank * 8) + nfile);
        }
    }
    return attacks;
}

bitBoard MoveGenerator::computePawnAttack(int square, int white) {
    bitBoard attacks = 0ULL;
    int file = square % 8;
    int rank = square / 8;
    int nrank, nfile;
    if (white) {
        //set white pawn attacking direction
        nrank = rank + 1;
        if (rank >= 8) {
            return attacks;
        }
    } else {
        //set black pawn attacking direction
        nrank = rank - 1;
        if (nrank < 0) {
            return attacks;
        }
    }
    nfile = file + 1;
    // check if diagonals are legal and set bit in attack board
    if (nfile < 8) {
        ChessBoard::setBit(attacks, (nrank * 8) + nfile);
    }
    nfile = file - 1;
    // check if diagonals are legal and set bit in attack board
    if (nfile >= 0) {
        ChessBoard::setBit(attacks, (nrank * 8) + nfile);
    }
    return attacks;
}

unsigned int MoveGenerator::XORShift32Rand() {
    static unsigned int state = 1804289383; // seed
    // XORShift32 algorithm
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}
bitBoard MoveGenerator::genRand64() {
    bitBoard n1, n2, n3, n4;
    n1 = XORShift32Rand() & 0xFFFF;
    n2 = XORShift32Rand() & 0xFFFF;
    n3 = XORShift32Rand() & 0xFFFF;
    n4 = XORShift32Rand() & 0xFFFF;
    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}
bitBoard MoveGenerator::getMagicNumberCandidate() {
    return genRand64() & genRand64() & genRand64();
}

bitBoard MoveGenerator::generateMagicNumber(int square, int rook) {
    bitBoard attackMask, attacks[4096], occupancies[4096], used[4096], magic;
    int bits, magicIndex, fail;
    // init attackmask
    attackMask = rook? genOccMask(square, 1) : genOccMask(square, 0);
    // count relevant bits
    bits = countBits(attackMask);
    // Calculate all occupancies and their corresponding attacks
    for (int i = 0; i < (1 << bits); i++) {
        occupancies[i] = setOccupancy(i, bits, attackMask);
        attacks[i] = rook? computeRookAttack(square, occupancies[i]) : computeBishopAttack(square, occupancies[i]);
    }
    for (int k = 0; k < 100000000; k++) {
        magic = getMagicNumberCandidate();
        // Discard irrelavant magic numbers
        if (countBits((attackMask * magic) & 0xFF00000000000000ULL) < 6) continue;
        for (int i = 0; i < 4096; i++) {
            used[i] = 0ULL;
        }
        fail = 0;
        for (int i = 0; !fail && i < (1 << bits); i++) {
            // Calculate Magic Index
            magicIndex = (occupancies[i] * magic) >> (64 - bits);
            // Mark the used indecies with the calculated attack
            if (used[magicIndex] == 0ULL) {
                used[magicIndex] = attacks[i];
            // if Collision is not appropriate discard magic number
            } else if (used[magicIndex] != attacks[i]) {
                fail = 1;
            }
        }
        if (!fail) {
            return magic;
        }
    }
    // No magic number found over __ amount of tries
    std::cout << "Error, No magic number found" <<std::endl;
    return -1;
}