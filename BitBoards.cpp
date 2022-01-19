#include "BitBoards.h"

// An array of bitboards of squares between two given square
// The between BB contains the square of the second index but not the first
bitBoard betweenBB[64][64];
bitBoard squares[64];
bitBoard pawnAttacks[2][64];
bitBoard knightAttacks[64];
bitBoard kingAttacks[64];
bitBoard rookAttacks[102400];
bitBoard bishopAttacks[5248];

MagicSquare rookMagics[64];
MagicSquare bishopMagics[64];


// Precompute attack tables and initialize magic bitboards
void BitBoards::precomputeAttackSets() {
    for (int i = 0; i < 64; i++) {
        bitBoard bb = 0;
        setBit(bb, i);
        squares[i] = bb;
        pawnAttacks[0][i] = computePawnAttack(i, 1);
        pawnAttacks[1][i] = computePawnAttack(i, 0);
        knightAttacks[i] = computeKnightAttack(i);
        kingAttacks[i] = computeKingAttack(i);
    }
    initRookMagics();
    initBishopMagics();
    initBetweenBB();
}

void BitBoards::initRookMagics() {
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

void BitBoards::initBishopMagics() {
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

void BitBoards::initBetweenBB() {
    int file, rank;
    for (int i = 0; i < 64; i++) {
        for (int k = 0; k < 64; k++) {
            bitBoard bb = 0;
            setBit(bb, k);
            betweenBB[i][k] = bb;
        }
    }
    for (int i = 0; i < 64; i++) {
        file = i % 8;
        rank = i / 8;
        
        for (int nrank = rank + 1; nrank < 8; nrank++) {
            betweenBB[i][8*nrank+file] = computeBetweenBB(i, 8*nrank+file, NORTH);
        }

        for (int nfile = file + 1; nfile < 8; nfile++) {
            betweenBB[i][8*rank+nfile] = computeBetweenBB(i, 8*rank+nfile, EAST);
        }

        for (int nrank = rank - 1; nrank >=0 ; nrank--) {
            betweenBB[i][8*nrank+file] = computeBetweenBB(i, 8*nrank+file, SOUTH);
        }

        for (int nfile = file - 1; nfile >=0; nfile--) {
            betweenBB[i][8*rank+nfile] = computeBetweenBB(i, 8*rank+nfile, WEST);
        }

        for (int nfile = file + 1, nrank = rank + 1; nfile < 8 && nrank < 8; nrank++, nfile++) {
            betweenBB[i][8*nrank+nfile] = computeBetweenBB(i, 8*nrank+nfile, NORTHEAST);
        }
        
        for (int nfile = file + 1, nrank = rank - 1; nfile < 8 && nrank >= 0; nrank--, nfile++) {
            betweenBB[i][8*nrank+nfile] = computeBetweenBB(i, 8*nrank+nfile, SOUTHEAST);
        }

        for (int nfile = file - 1, nrank = rank - 1; nfile >= 0 && nrank >= 0; nrank--, nfile--) {
            betweenBB[i][8*nrank+nfile] = computeBetweenBB(i, 8*nrank+nfile, SOUTHWEST);
        }

        for (int nfile = file - 1, nrank = rank + 1; nfile >= 0 && nrank < 8; nrank++, nfile--) {
            betweenBB[i][8*nrank+nfile] = computeBetweenBB(i, 8*nrank+nfile, NORTHWEST);
        }

    }
}

bitBoard BitBoards::computeBetweenBB(int sq1, int sq2, Direction dir) {
    bitBoard betweenBB = 0;
    while (sq1 != sq2) {
        sq1 += dir;
        setBit(betweenBB, sq1);
    }
    return betweenBB;
}

// computes the rook attack for a given occupancy. Goes through all valid rook directions (north east etc.) until a set bit
// is found in the occupancy board
bitBoard BitBoards::computeRookAttack(int square, bitBoard occ) {
    bitBoard attacks = 0;
    bitBoard curr;
    int file = square % 8;
    int rank = square / 8;
    for (int nrank = rank + 1; nrank < 8; nrank++) {
        curr = 0;
        setBit(curr, (8 * nrank) + file);
        attacks |= curr;
        if (occ & curr) break;
    }
    for (int nfile = file + 1; nfile < 8; nfile++) {
        curr = 0;
        setBit(curr, (8 * rank) + nfile);
        attacks |= curr;
        if (occ & curr) break;
    }
    for (int nrank = rank - 1; nrank >=  0; nrank--) {
        curr = 0;
        setBit(curr, (8 * nrank) + file);
        attacks |= curr;
        if (occ & curr) break;
    }
    for (int nfile = file - 1; nfile >= 0; nfile--) {
        curr = 0;
        setBit(curr, (8 * rank) + nfile);
        attacks |= curr;
        if (occ & curr) break;
    }
    return attacks;
}

// Computes Bishop Attack for an occupancy. Goes through all valid bishop directions (northWest northEast, etc) one by one until
// a set bit is found in the occupancy board.
bitBoard BitBoards::computeBishopAttack(int square, bitBoard occ) {
    bitBoard attacks = 0;
    int file = square % 8;
    int rank = square / 8;
    bitBoard curr;
    for (int nrank = rank + 1, nfile = file + 1; nrank < 8 && nfile < 8; nrank++, nfile++) {
        curr = 0;
        setBit(curr, (8 * nrank) + nfile);
        attacks |= curr;
        if (occ & curr) break;
    }
    for (int nrank = rank + 1, nfile = file - 1; nrank < 8 && nfile >= 0; nrank++, nfile--) {
        curr = 0;
        setBit(curr, (8 * nrank) + nfile);
        attacks |= curr;
        if (occ & curr) break;
    }
    for (int nrank = rank - 1, nfile = file - 1; nrank >= 0 && nfile >= 0; nrank--, nfile--) {
        curr = 0;
        setBit(curr, (8 * nrank) + nfile);
        attacks |= curr;
        if (occ & curr) break;
    }
    for (int nrank = rank - 1, nfile = file + 1; nrank >=0  && nfile < 8; nrank--, nfile++) {
        curr = 0;
        setBit(curr, (8 * nrank) + nfile);
        attacks |= curr;
        if (occ & curr) break;
    }
    return attacks;
}

bitBoard BitBoards::computeKnightAttack(int square) {
    bitBoard attacks = 0ULL;
    int file = square % 8;
    int rank = square / 8;
    int nrank, nfile;
    nrank = rank + 2;
    if (nrank < 8) {
        nfile = file + 1;
        if (nfile < 8) {
            setBit(attacks, (nrank * 8) + nfile);
        }
        nfile = file - 1;
        if (nfile >= 0) {
            setBit(attacks, (nrank * 8) + nfile);
        }
    }
    nfile = file + 2;
    if (nfile < 8) {
        nrank = rank + 1;
        if (nrank < 8) {
            setBit(attacks, (nrank * 8) + nfile);
        }
        nrank = rank - 1;
        if (nrank >= 0) {
            setBit(attacks, (nrank * 8) + nfile);
        }
    }
    nrank = rank - 2;
    if (nrank >= 0) {
        nfile = file + 1;
        if (nfile < 8) {
            setBit(attacks, (nrank * 8) + nfile);
        }
        nfile = file - 1;
        if (nfile >= 0) {
            setBit(attacks, (nrank * 8) + nfile);
        }
    }
    nfile = file - 2;
    if (nfile >= 0) {
        nrank = rank + 1;
        if (nrank < 8) {
            setBit(attacks, (nrank * 8) + nfile);
        }
        nrank = rank - 1;
        if (nrank >= 0) {
            setBit(attacks, (nrank * 8) + nfile);
        }
    }
    return attacks;
}

bitBoard BitBoards::computeKingAttack(int square) {
    bitBoard attacks = 0ULL;
    int file = square % 8;
    int rank = square / 8;
    int nrank, nfile;
    nrank = rank + 1;
    if (nrank < 8) {
        nfile = file - 1;
        if (nfile >= 0) {
            setBit(attacks, (nrank * 8) + nfile);
        }
        nfile = file;
        setBit(attacks, (nrank * 8) + nfile);
        nfile = file + 1;
        if (nfile < 8) {
            setBit(attacks, (nrank * 8) + nfile);
        }
    }
    nrank = rank;
    nfile = file - 1;
    if (nfile >= 0) {
        setBit(attacks, (nrank * 8) + nfile);
    }
    nfile = file + 1;
    if (nfile < 8) {
        setBit(attacks, (nrank * 8) + nfile);
    }
    nrank = rank - 1;
    if (nrank >= 0) {
        nfile = file - 1;
        if (nfile >= 0) {
            setBit(attacks, (nrank * 8) + nfile);
        }
        nfile = file;
        setBit(attacks, (nrank * 8) + nfile);
        nfile = file + 1;
        if (nfile < 8) {
            setBit(attacks, (nrank * 8) + nfile);
        }
    }
    return attacks;
}

bitBoard BitBoards::computePawnAttack(int square, int white) {
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
        setBit(attacks, (nrank * 8) + nfile);
    }
    nfile = file - 1;
    // check if diagonals are legal and set bit in attack board
    if (nfile >= 0) {
        setBit(attacks, (nrank * 8) + nfile);
    }
    return attacks;
}

bitBoard BitBoards::genOccMask(int square, int rook) {
    bitBoard occMask = 0;
    int file = square % 8;
    int rank = square / 8;
    if (rook) {
        for (int nfile = file + 1; nfile < 7; nfile++) setBit(occMask, (8 * rank) + nfile);
        for (int nrank = rank + 1; nrank < 7; nrank++) setBit(occMask, (8* nrank) + file);
        for (int nfile = file - 1; nfile > 0; nfile--) setBit(occMask, (8 * rank) + nfile);
        for (int nrank = rank - 1; nrank > 0; nrank--) setBit(occMask, (8* nrank) + file);
    } else {
        for (int nfile = file + 1, nrank = rank + 1; nfile < 7 && nrank < 7; nrank++, nfile++)
            setBit(occMask, (8 * nrank) + nfile);
        for (int nfile = file + 1, nrank = rank - 1; nfile < 7 && nrank > 0; nrank--, nfile++)
            setBit(occMask, (8 * nrank) + nfile);
        for (int nfile = file - 1, nrank = rank - 1; nfile > 0 && nrank > 0; nrank--, nfile--)
            setBit(occMask, (8 * nrank) + nfile);
        for (int nfile = file - 1, nrank = rank + 1; nfile > 0 && nrank < 7; nrank++, nfile--)
            setBit(occMask, (8 * nrank) + nfile);
    }
    return occMask;
}

bitBoard BitBoards::setOccupancy(int index, int bitsInMask, bitBoard attackMask) {
    bitBoard occ = 0;

    for (int count = 0; count < bitsInMask; count++) {
        int bit = popLSB(attackMask);
        if (index & (1 << count)) {
            setBit(occ, bit);
        }
    }

    return occ;
}