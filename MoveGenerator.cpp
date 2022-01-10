#include "MoveGenerator.h"

constexpr bitBoard MoveGenerator::bishopMagics[64];
constexpr bitBoard MoveGenerator::rookMagics[64];

MoveGenerator::MoveGenerator() {
    initRookOccMask();
    initBishopOccMask();
    precomputeAttackSets();
}

Move* MoveGenerator::generateMoves(ChessBoard* chessBoard, Move* moves) {
    initEnemyAttacks(*chessBoard);
    moves = generateKingMoves(*chessBoard, moves);
    moves = generateSlidingMoves(*chessBoard, moves);
    moves = generateKnightMoves(*chessBoard, moves);
    moves = generatePawnMoves(*chessBoard, moves);
    return moves;
}

Move* MoveGenerator::generateSlidingMoves(ChessBoard& chessBoard, Move* moves) {
    bitBoard notAllies, rooks, bishops, queens, maskedOcc, attacks;
    int from, to;
    if (chessBoard.whiteToMove) {
        notAllies = ~chessBoard.piecesByColour[WHITE];
        rooks = chessBoard.piecesByType[WHITEROOK];
        queens = chessBoard.piecesByType[WHITEQUEEN];
        bishops = chessBoard.piecesByType[WHITEBISHOP];
    } else {
        notAllies = ~chessBoard.piecesByColour[BLACK];
        rooks = chessBoard.piecesByType[BLACKROOK];
        queens = chessBoard.piecesByType[BLACKQUEEN];
        bishops = chessBoard.piecesByType[BLACKBISHOP];
    }

    while (rooks) {
        from = getLSBIndex(rooks);
        rooks ^= (bitBoard) 1 << from;
        maskedOcc = rookOccMask[from] & chessBoard.allPieces;
        attacks = rookAttacks[from][maskedOcc * rookMagics[from] >> (64 - bitsInRookMask[from])];
        attacks &= notAllies;
        while (attacks) {
            to = getLSBIndex(attacks);
            attacks ^= (bitBoard)1 << to;
            moves->captToFrom = (chessBoard.pieceOn(to) << 12) + (to << 6) + from;
            moves->promFlags = 0;
            moves++;
        }
    }

    while (bishops) {
        from = getLSBIndex(bishops);
        bishops ^= (bitBoard)1 << from;
        maskedOcc = bishopOccMask[from] & chessBoard.allPieces;
        attacks = bishopAttacks[from][maskedOcc * bishopMagics[from] >> (64 - bitsInBishopMask[from])];
        attacks &= notAllies;
        while (attacks) {
            to = getLSBIndex(attacks);
            attacks ^= (bitBoard)1 << to;
            moves->captToFrom = (chessBoard.pieceOn(to) << 12) + (to << 6) + from;
            moves->promFlags = 0;
            moves++;
        }
    }

    while (queens) {
        from = getLSBIndex(queens);
        queens ^= (bitBoard) 1 << from;
        maskedOcc = rookOccMask[from] & chessBoard.allPieces;
        attacks = rookAttacks[from][maskedOcc * rookMagics[from] >> (64 - bitsInRookMask[from])];
        maskedOcc = bishopOccMask[from] & chessBoard.allPieces;
        attacks |= bishopAttacks[from][maskedOcc * bishopMagics[from] >> (64 - bitsInBishopMask[from])];
        attacks &= notAllies;
        while (attacks) {
            to = getLSBIndex(attacks);
            attacks ^= (bitBoard)1 << to;
            moves->captToFrom = (chessBoard.pieceOn(to) << 12) + (to << 6) + from;
            moves->promFlags = 0;
            moves++;
        }
    }
    return moves;
}

Move* MoveGenerator::generateKnightMoves(ChessBoard& chessBoard, Move* moves) {
    bitBoard notAllies, knights, attacks;
    int from, to;
    if (chessBoard.whiteToMove) {
        notAllies = ~chessBoard.piecesByColour[WHITE];
        knights = chessBoard.piecesByType[WHITEKNIGHT];
    } else {
        notAllies = ~chessBoard.piecesByColour[BLACK];
        knights = chessBoard.piecesByType[BLACKKNIGHT];
    }
    while(knights) {
        from = getLSBIndex(knights);
        knights ^= (bitBoard) 1 << from;
        attacks = knightAttacks[from] & notAllies;
        while (attacks) {
            to = getLSBIndex(attacks);
            attacks ^= (bitBoard)1 << to;
            moves->captToFrom = (chessBoard.pieceOn(to) << 12) + (to << 6) + from;
            moves->promFlags = 0;
            moves++;
        }
    }
    return moves;
}

Move* MoveGenerator::generateKingMoves(ChessBoard& chessBoard, Move* moves) {
    bitBoard notAllies, king, attacks;
    int from, to;
    if (chessBoard.whiteToMove) {
        notAllies = ~chessBoard.piecesByColour[WHITE];
        king = chessBoard.piecesByType[WHITEKING];
    } else {
        notAllies = ~chessBoard.piecesByColour[BLACK];
        king = chessBoard.piecesByType[BLACKKING];
    }

    while(king) {
        from = getLSBIndex(king);
        king ^= (bitBoard)1 << from;
        attacks = kingAttacks[from] & notAllies;
        while (attacks) {
            to = getLSBIndex(attacks);
            attacks ^= (bitBoard) 1 << to;
            moves->captToFrom = (chessBoard.pieceOn(to) << 12) + (to << 6) + from;
            moves->promFlags = 0;
            moves++;
        }
    }
    return moves;
}

Move* MoveGenerator::generatePawnMoves(ChessBoard& chessBoard, Move* moves) {
    bitBoard* pawnColourAttacks;
    bitBoard pawns, enemies, attacks, singlePush, doublePush, epAttacks, promotions;
    int from, to, forward;
    int epSquare = chessBoard.enPassentSquare;
    Colour colour;
    if (chessBoard.whiteToMove) {
        pawns = chessBoard.piecesByType[WHITEPAWN];
        promotions = pawns & 0xff000000000000;
        pawns ^= promotions;
        singlePush = (pawns << 8) & ~chessBoard.allPieces;
        doublePush = ((singlePush & 0xff0000) << 8) & ~chessBoard.allPieces;
        enemies = chessBoard.piecesByColour[BLACK];
        colour = WHITE;
        if (epSquare >= 0) {
            epAttacks = pawnAttacks[BLACK][epSquare] & pawns;
        } else {
            epAttacks = (bitBoard)0;
        }
        // The attack array to reference
        pawnColourAttacks = pawnAttacks[WHITE];
        forward = 8;
    } else {
        pawns = chessBoard.piecesByType[BLACKPAWN];
        promotions = pawns & 0xff00;
        pawns ^= promotions;
        singlePush = (pawns >> 8) & ~chessBoard.allPieces;
        doublePush = ((singlePush & 0xff0000000000) >> 8) & ~chessBoard.allPieces;
        enemies = chessBoard.piecesByColour[WHITE];
        colour = BLACK;
        // The attack array to reference
        pawnColourAttacks = pawnAttacks[BLACK];
        forward = -8;
        if (epSquare >= 0) {
            epAttacks = pawnAttacks[WHITE][epSquare] & pawns;
        } else {
            epAttacks = (bitBoard)0;
        }
    }
    // gen Pawn Attacks
    while (pawns) {
        from = getLSBIndex(pawns);
        pawns ^= (bitBoard) 1 << from;
        attacks = pawnColourAttacks[from] & enemies;
        while (attacks) {
            to = getLSBIndex(attacks);
            attacks ^= (bitBoard) 1 << to;
            moves->captToFrom = (chessBoard.pieceOn(to) << 12) + (to << 6) + from;
            moves->promFlags = 0;
            moves++;
        }
    }
    //gen Pawn Pushes
    while (singlePush) {
        to = getLSBIndex(singlePush);
        singlePush ^= (bitBoard) 1 << to;
        moves->captToFrom = (to << 6) + to - forward;
        moves->promFlags = 0;
        moves++;
    }
    //gen Double Pawn Pushes
    while (doublePush) {
        to = getLSBIndex(doublePush);
        doublePush ^= (bitBoard) 1 << to;
        moves->captToFrom = (to << 6) + to -  (2 * forward);
        moves->promFlags = DOUBLEPUSH;
        moves++;
    }
    //gen enPassent
    while (epAttacks) {
        from = getLSBIndex(epAttacks);
        epAttacks ^= (bitBoard) 1 << from;
        moves->captToFrom = (epSquare << 6) + from;
        moves->promFlags = ENPASSENT;
        moves++;
    }
    //gen Promotions
    if (promotions) {
        bitBoard pushedProms = pushUp(promotions, colour) & ~chessBoard.piecesByColour[colour];
        from = getLSBIndex(promotions);
        promotions ^= (bitBoard)1 << from;
        attacks = pawnColourAttacks[from] & enemies;
        while (attacks) {
            to = getLSBIndex(attacks);
            attacks ^= (bitBoard)1 << to;
            int captToFrom = (chessBoard.pieceOn(to) << 12) + (to << 6) + from;
            for (int i = KNIGHT; i < KING; i++) {
                moves->captToFrom = captToFrom;
                moves->promFlags = (i + (colour << 3)) << 4;
                moves++;
            }
        }
        while (pushedProms) {
            to = getLSBIndex(pushedProms);
            pushedProms ^= (bitBoard)1 << to;
            int captToFrom = (chessBoard.pieceOn(to) << 12) + (to << 6) + from;
            for (int i = KNIGHT; i < KING; i++) {
                moves->captToFrom = captToFrom;
                moves->promFlags = (i + (colour << 3)) << 4;
                moves++;
            }
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

void MoveGenerator::initEnemyAttacks(ChessBoard& chessBoard) {
    attackedByEnemy = (bitBoard)0;

}

void MoveGenerator::initRookOccMask() {
    for (int i = 0; i < 64; i++) {
        rookOccMask[i] = genOccMask(i, 1);
        bitsInRookMask[i] = ChessBoard::countBits(rookOccMask[i]);
    }
}

void MoveGenerator::initBishopOccMask() {
    for (int i = 0; i < 64; i++) {
        bishopOccMask[i] = genOccMask(i, 0);
        bitsInBishopMask[i] = ChessBoard::countBits(bishopOccMask[i]);
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
        int bit = getLSBIndex(attackMask);
        ChessBoard::popBit(attackMask, bit);
        if (index & (1 << count)) {
            ChessBoard::setBit(occ, bit);
        }
    }

    return occ;
}

// Precompute attack tables and initialize magic bitboards
void MoveGenerator::precomputeAttackSets() {
    bitBoard occ, attack, magicIndex;
    for (int i = 0; i < 64; i++) {
        pawnAttacks[0][i] = computePawnAttack(i, 1);
        pawnAttacks[1][i] = computePawnAttack(i, 0);
        knightAttacks[i] = computeKnightAttack(i);
        kingAttacks[i] = computeKingAttack(i);
        for (int k = 0; k < (1 << bitsInRookMask[i]); k++) {
            occ = setOccupancy(k, bitsInRookMask[i], rookOccMask[i]);
            attack = computeRookAttack(i, occ);
            magicIndex = occ * rookMagics[i] >> (64 - bitsInRookMask[i]);
            rookAttacks[i][magicIndex] = attack;
        }
        for (int k = 0; k < (1 << bitsInBishopMask[i]); k++) {
            occ = setOccupancy(k, bitsInBishopMask[i], bishopOccMask[i]);
            attack = computeBishopAttack(i, occ);
            magicIndex = occ * bishopMagics[i] >> (64 - bitsInBishopMask[i]);
            bishopAttacks[i][magicIndex] = attack;
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
    if (file < 8) {
        ChessBoard::setBit(attacks, (nrank * 8) + nfile);
    }
    nfile = file - 1;
    // check if diagonals are legal and set bit in attack board
    if (file >= 0) {
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
    attackMask = rook? rookOccMask[square] : bishopOccMask[square];
    // count relevant bits
    bits = ChessBoard::countBits(attackMask);
    // Calculate all occupancies and their corresponding attacks
    for (int i = 0; i < (1 << bits); i++) {
        occupancies[i] = setOccupancy(i, bits, attackMask);
        attacks[i] = rook? computeRookAttack(square, occupancies[i]) : computeBishopAttack(square, occupancies[i]);
    }
    for (int k = 0; k < 100000000; k++) {
        magic = getMagicNumberCandidate();
        // Discard irrelavant magic numbers
        if (ChessBoard::countBits((attackMask * magic) & 0xFF00000000000000ULL) < 6) continue;
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